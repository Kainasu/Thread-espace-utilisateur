import sys
import matplotlib.pyplot as plt
import os

from sympy import E

limit_nb_thread = 100
img_dir = "graphs/imgs/"


get_number_of_threads = " | grep en | grep us | awk '{print $1;}'"
get_exec_time = " | grep en | grep us | awk '{print $(NF-1);}'"

install_bin = "LD_LIBRARY_PATH=./install/lib ./install/bin/"
install_pthread = "./install/pthread/"

def test_graph(test_name,nbthreads=limit_nb_thread,threads_step=100,mean_range=20):    
    exec_lib = install_bin + test_name
    exec_pthread = install_pthread + test_name + "-pthread"
    threads = []
    lib_times = []
    pthread_times=[]
    mean_range=20
    for i in range (5, nbthreads, threads_step):
        threads += [i]
        lib_time=0
        pthread_time=0
        for k in range(mean_range):                        
            lib_time += int(os.popen(exec_lib + " " +  str(i) + get_exec_time).read())*10**(-3)                        
            pthread_time += int(os.popen(exec_pthread + " " +  str(i) + get_exec_time).read())*10**(-3) 

        lib_times.append(lib_time/mean_range)
        pthread_times.append(pthread_time/mean_range)

    return threads, pthread_times, lib_times



def test_graph_yield(test_name,nbthreads=limit_nb_thread,threads_step=100,mean_range=20,nb_yields=20):
    exec_lib = install_bin + test_name
    exec_pthread = install_pthread + test_name + "-pthread"

    threads = []
    lib_times = []
    pthread_times=[]
    mean_range=20
    for i in range (    5, nbthreads, threads_step):
        threads += [i]        
        lib_time=0
        pthread_time=0
        for k in range(mean_range):                      
            lib_time += float(os.popen(exec_lib + " " +  str(i) + " " + str(nb_yields) + "| grep en | tail -n 1 | awk '{print $(NF-1);}'").read())*10**(-3)            
            pthread_time += float(os.popen(exec_pthread + " " + str(i) + " " + str(nb_yields) + "|  grep en | tail -n 1 | awk '{print $(NF-1);}'").read())*10**(-3)

        lib_times.append(lib_time/mean_range)
        pthread_times.append(pthread_time/mean_range)

    return threads, pthread_times, lib_times



def draw_graph(test_name, threads_range, thread_data, pthread_data, using_yield=False):        
    plt.figure(test_name+"_graph")
    if (using_yield):
        plt.title(test_name+"_graph_yields")
    else :
        plt.title(test_name+"_graph")
    
    
    plt.plot(threads_range, thread_data, c='b', label="thread")
    plt.plot(threads_range, pthread_data, c='r', label="pthread")
    plt.xlabel('Number')
    
    plt.ylabel('Execution Time (ms)')
    plt.legend()
    plt.savefig(img_dir+test_name+'.png')
    plt.clf()

def get_graph(test_name, nbthreads=limit_nb_thread, threads_step=5,  mean_range=20,yields_max=None):
    if (yields_max is None):
        using_yield = False
    else:
        using_yield = True
    
    if (using_yield):
        threads_range, pthread_data, thread_data = test_graph_yield(test_name,nbthreads,threads_step,mean_range, yields_max)
    else:
        threads_range, pthread_data, thread_data = test_graph(test_name,nbthreads,threads_step, mean_range)


    draw_graph(test_name, threads_range, thread_data, pthread_data, using_yield)
    

def init():
    if not os.path.exists(img_dir):
        os.makedirs(img_dir)


#affichage des graphes de comparaisons
if __name__ == "__main__":
    init()
    
    print("Exporting 21-create-many graph ")
    get_graph("21-create-many", 200,100,20)
    print("Exporting 22-create-many-recursive graph ")
    get_graph("22-create-many-recursive", 200,10,2)
    print("Exporting 31-switch-many graph ")
    get_graph("31-switch-many", 100,1,20,100)
    print("Exporting 32-switch-many-join graph")
    get_graph("32-switch-many-join", 100,1,20,100)
    print("Exporting 33-switch-many-cascade graph")
    get_graph("33-switch-many-cascade", 10, 1, 20, 10000)
    print("Exporting 51-fibonnaci")
    get_graph("51-fibonacci", 20, 1, 100, 30)
    print("Exporting 61-mutex")
    get_graph("61-mutex", 40, 10, 5, 20)    
    print("Exporting 62-mutex")
    get_graph("62-mutex", 40, 10, 5, 20)
    print("All graphs are generated")













