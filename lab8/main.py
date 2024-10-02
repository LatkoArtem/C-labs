import ctypes
import os


def load_library(library_name):
    return ctypes.CDLL(os.path.abspath(library_name))


def get_int_input(prompt):
    while True:
        try:
            return int(input(prompt))
        except ValueError:
            print("Invalid input! Please enter an integer.")


def get_float_input(prompt):
    while True:
        try:
            return float(input(prompt))
        except ValueError:
            print("Invalid input! Please enter a float.")


def execute_lab4():
    lib = load_library("liblab4.so")

    lib.distance.restype = ctypes.c_float
    lib.points_inside_circle.restype = ctypes.c_int
    lib.generate_points.argtypes = [ctypes.POINTER(ctypes.c_float), ctypes.POINTER(ctypes.c_float)]
    lib.binomial_coefficient.restype = ctypes.c_int
    lib.binomial_coefficient.argtypes = [ctypes.c_int, ctypes.c_int]

    while True:
        choice = get_int_input("Choose a task (1 or 2): ")
        if choice == 1:
            X = (ctypes.c_float * 10)()
            Y = (ctypes.c_float * 10)()
            
            lib.generate_points(X, Y)
            print("Randomly generated points: ")
            for i in range(10):
                print(f"({round(X[i], 2)}, {round(Y[i], 2)}) ")
            while True:
                radius = get_float_input("Enter the radius of the circle: ")
                result = lib.points_inside_circle(X, Y, ctypes.c_float(radius))
                
                if result:
                    break
                else:
                    print(f"The number of points inside the circle: {result}")
                    break
            break
        elif choice == 2:
            m = get_int_input("Enter m: ")
            n = get_int_input("Enter n: ")
            result = lib.binomial_coefficient(m, n)
            print(f"Binomial coefficient: C({m}, {n}) = {result}")
        else:
            print("Incorrect input!")
            continue


def execute_lab5():
    lib = load_library("liblab5.so")
    
    class Route(ctypes.Structure):
        _fields_ = [("start", ctypes.c_char * 50), ("end", ctypes.c_char * 50),
                    ("route_number", ctypes.c_int)]
    
    routes = (Route * 5)()
    for i in range(5):
        while True:
            start = input(f"Enter the name of the starting point of the route {i + 1}: ")
            if start.strip():
                None
            else:
                print("Incorrect input!")
                continue
            break
        while True:
            end = input(f"Enter the name of the end point of the route {i + 1}: ")
            if end.strip():
                None
            else:
                print("Incorrect input!")
                continue
            break
        route_number = get_int_input(f"Enter the route number of the route {i + 1}: ")
        routes[i].start = start.encode('utf-8')
        routes[i].end = end.encode('utf-8')
        routes[i].route_number = route_number
    
    lib.sort_routes(routes, 5)
    
    print("\nSorted routes:")
    print("|{:<15} |{:<15} |{:<10}|".format("Start", "Finish", "Route number"))
    print("|________________|________________|____________|")
    for route in routes:
        print("|{:<15} |{:<15} |{:<10}  |".format(route.start.decode(
            'utf-8'), route.end.decode('utf-8'), route.route_number))
    
    while True:
        search_route = get_int_input(
            "Enter the number of the route you want to find, or enter 0 to end the search: ")
        if search_route < 0:
            print("Incorrect input!")
        else:
            if search_route == 0:
                break
            
            result = Route()
            if lib.find_route(routes, 5, search_route, ctypes.byref(result)) == 0:
                print(
                    f"Found route №{search_route}: {result.start.decode('utf-8')} - {result.end.decode('utf-8')}")
            else:
                print("No such route found.")
                continue


if __name__ == "__main__":
    while True:
        task = get_int_input("Enter the number of the laboratory work (4 or 5): ")
        if task == 4:
            execute_lab4()
            break
        elif task == 5:
            execute_lab5()
            break
        else:
            print("Incorrect input!")
            continue