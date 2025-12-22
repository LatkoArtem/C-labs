import tkinter as tk
from tkinter import messagebox
import threading, time, random

# ------------------ Shared state ------------------
rects = []
rects_lock = threading.Lock()
rect_limit = 600

thread_infos = []
thread_infos_lock = threading.Lock()

colors = [
    "#4CAF50", "#00BCD4", "#FFC107", "#E91E63",
    "#9C27B0", "#FF5722", "#8BC34A", "#03A9F4"
]

use_lock = True  # Sync ON/OFF flag
base_speed = 0.5  # seconds per iteration

# ------------------ ThreadInfo ------------------
class ThreadInfo:
    def __init__(self, idx):
        self.idx = idx
        self.thread = None
        self.alive = True
        self.suspended = False
        self.priority = 0  # -2..2
        self.color = colors[idx % len(colors)]

# ------------------ Worker ------------------
def worker_loop(info, canvas_w, canvas_h):
    while info.alive:
        while info.suspended and info.alive:
            time.sleep(0.1)

        w = random.randint(20, 120)
        h = random.randint(20, 120)
        x = random.randint(0, max(0, canvas_w - w))
        y = random.randint(0, max(0, canvas_h - h))
        color = info.color

        if use_lock:
            with rects_lock:
                rects.append((x, y, w, h, color))
                if len(rects) > rect_limit:
                    rects.pop(0)
        else:
            rects.append((x, y, w, h, color))
            if len(rects) > rect_limit:
                rects.pop(0)

        # Sleep only based on base_speed, priority більше не впливає
        time.sleep(base_speed)

# ------------------ GUI actions ------------------
def create_thread():
    with thread_infos_lock:
        idx = len(thread_infos)
        info = ThreadInfo(idx)
        thread_infos.append(info)

    t = threading.Thread(target=worker_loop,
                         args=(info, canvas.winfo_width(), canvas.winfo_height()),
                         daemon=True)
    info.thread = t
    t.start()
    refresh_thread_list()

def _get_selected_info():
    sel = listbox.curselection()
    if not sel: return None, None
    idx = sel[0]
    with thread_infos_lock:
        if idx < 0 or idx >= len(thread_infos): return None, None
        return idx, thread_infos[idx]

def terminate_selected():
    idx, info = _get_selected_info()
    if info is None: return
    info.alive = False
    refresh_thread_list()

def suspend_resume_selected():
    idx, info = _get_selected_info()
    if info is None: return
    info.suspended = not info.suspended
    refresh_thread_list()

def priority_up():
    idx, info = _get_selected_info()
    if info is None: return
    if info.priority < 2: info.priority += 1
    refresh_thread_list()

def priority_down():
    idx, info = _get_selected_info()
    if info is None: return
    if info.priority > -2: info.priority -= 1
    refresh_thread_list()

def toggle_sync():
    global use_lock
    use_lock = not use_lock
    btn_sync.config(text=f"Sync: {'ON' if use_lock else 'OFF'}")

def set_base_speed(val):
    global base_speed
    base_speed = float(val)

# ------------------ UI refresh/draw loops ------------------
def refresh_thread_list():
    sel = listbox.curselection()
    sel_idx = sel[0] if sel else None

    listbox.delete(0, tk.END)
    with thread_infos_lock:
        for info in thread_infos:
            status = "ALIVE" if info.alive else "DEAD"
            susp = "SUSP" if info.suspended else ""
            listbox.insert(tk.END, f"[{info.idx}] PRI={info.priority} {status} {susp}")

    if sel_idx is not None and sel_idx < listbox.size():
        listbox.select_set(sel_idx)

def periodic_refresh():
    refresh_thread_list()
    root.after(300, periodic_refresh)

def draw_loop():
    canvas.delete("all")
    if use_lock:
        with rects_lock:
            local = list(rects)
    else:
        local = list(rects)

    for (x, y, w, h, c) in local:
        canvas.create_rectangle(x, y, x + w, y + h, fill=c, outline="")
    root.after(50, draw_loop)

# ------------------ Main UI ------------------
root = tk.Tk()
root.title("Multithread Demo (Python)")
root.geometry("950x540")

frame_left = tk.Frame(root)
frame_left.pack(side=tk.LEFT, fill=tk.Y, padx=8, pady=8)

btn_create = tk.Button(frame_left, text="Create Thread", width=18, command=create_thread)
btn_create.pack(pady=4)
btn_term = tk.Button(frame_left, text="Terminate", width=18, command=terminate_selected)
btn_term.pack(pady=4)
btn_susp = tk.Button(frame_left, text="Suspend/Resume", width=18, command=suspend_resume_selected)
btn_susp.pack(pady=4)
btn_pup = tk.Button(frame_left, text="Priority +", width=18, command=priority_up)
btn_pup.pack(pady=4)
btn_pdown = tk.Button(frame_left, text="Priority -", width=18, command=priority_down)
btn_pdown.pack(pady=4)
btn_sync = tk.Button(frame_left, text="Sync: ON", width=18, command=toggle_sync)
btn_sync.pack(pady=6)
tk.Label(frame_left, text="Base speed (sec):").pack(pady=(10,0))
speed_slider = tk.Scale(frame_left, from_=0.1, to=2.0, resolution=0.05,
                        orient=tk.HORIZONTAL, command=set_base_speed)
speed_slider.set(base_speed)
speed_slider.pack()
btn_exit = tk.Button(frame_left, text="Exit", width=18, command=root.quit)
btn_exit.pack(pady=20)

listbox = tk.Listbox(frame_left, width=36, height=20)
listbox.pack(pady=6)

canvas = tk.Canvas(root, width=600, height=480, bg="black")
canvas.pack(side=tk.RIGHT, padx=8, pady=8)

# ------------------ Start loops ------------------
root.after(50, draw_loop)
root.after(100, periodic_refresh)

root.mainloop()

# ------------------ Cleanup ------------------
with thread_infos_lock:
    for info in thread_infos:
        info.alive = False
