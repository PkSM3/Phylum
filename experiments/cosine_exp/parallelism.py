import random
import time
import sys
from multiprocessing import Process, Queue
random.seed()

# # SOURCE: https://www.praetorian.com/blog/multi-core-and-distributed-programming-in-python

def genList (size):
	randomList = []
	for i in range(size):
		randomList.append(random.randint(0,10))
	return randomList


def sumList(inList):
	finalSum = 0
	for value in inList:
		finalSum = finalSum + value
	return finalSum

def doWork(N,q):
	#create a random list of N integers
	myList = genList (N)
	finalSum = sumList(myList)
	#put the result in the Queue to return the the calling process
	q.put(finalSum)


if __name__ == '__main__':
	if len(sys.argv) == 2 and sys.argv[1].isdigit():
		N = int(sys.argv[1])
		#mark the start time
		startTime = time.time()
		#create a Queue to share results
		q = Queue()

		jobs = []
		for i in range(4):
			j = Process(target=doWork, args=(N/4,q))
			jobs.append( j )
			j.start()
		# #create 4 sub-processes to do the work
		# p1 = Process(target=doWork, args=(N/4,q))
		# p1.start()
		# p2 = Process(target=doWork, args=(N/4,q))
		# p2.start()
		# p3 = Process(target=doWork, args=(N/4,q))
		# p3.start()
		# p4 = Process(target=doWork, args=(N/4,q))
		# p4.start()
		 
		results = []
		#grab 4 values from the queue, one for each process
		for i in range(4):
			#set block=True to block until we get a result
			results.append(q.get(True))
		 
		#sum the partial results to get the final result
		finalSum = sumList(results)
		
		for j in jobs:
			j.join()
		# p1.join()
		# p2.join()
		# p3.join()
		# p4.join()
		print results
		#mark the end time
		endTime = time.time()
		#calculate the total time it took to complete the work
		workTime =  endTime - startTime
		 
		#print results
		print "The job took " + str(workTime) + " seconds to complete"
		print "The final sum was: " + str(finalSum)
	else:
		print "ERROR"
		exit(-1)