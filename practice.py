import threading
import time
from queue import Queue
import sys
import os

#directories=[]

#rootDir = '.'
#for dirName, subdirList, fileList in os.walk(rootDir):
    #directories.append(os.path.abspath(dirName))

print_lock=threading.Lock()
q_lock=threading.Lock()

def exampleJob(directory):
    everything=os.listdir(directory)
    onlyfiles = []
    for f in everything:
        fullpath=os.path.join(directory,f)
        if os.path.isdir(fullpath):
            q.put(fullpath)
        else:
            onlyfiles.append(f)
    with print_lock:
        print(directory, onlyfiles)
        #print(threading.current_thread().name, directories[worker])
        
def threader():
    while True:
        #q_lock.acquire()
        worker=q.get() #Lines above and below are needed in C
        #q_lock.release()
        exampleJob(worker)
        #q_lock.acquire()
        q.task_done()
        #q_lock.release()

q=Queue()

for x in range(int(sys.argv[1])):
    t = threading.Thread(target=threader)
    t.daemon=True
    t.start()
    
start=time.time()

#for worker in range(len(directories)):
    #with q_lock:
q.put(os.getcwd())
    
q.join()

print('Entire job took:', time.time()-start)
