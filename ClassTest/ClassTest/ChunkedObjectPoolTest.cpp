
#include "ChunkedObjectPool.h"
#include <chrono>
#include <stdio.h>

void RunChunkedObjectPoolTest()
{
	ChunkedObjectPool<STest, 200> myPool;


	using std::chrono::high_resolution_clock;

	printf("Starting Get() Test from scratch:\n");

	unsigned int nObjectsPerTake = 5000;
	unsigned int nTakes = 30;
	for (unsigned int n = 0; n < nTakes; ++n)
	{
		high_resolution_clock::time_point t0 = high_resolution_clock::now();
		for (unsigned int i = 0; i < nObjectsPerTake; ++i)
		{
			STest* pInstance = myPool.Get();
		}

		high_resolution_clock::time_point t1 = high_resolution_clock::now();
		std::chrono::duration<double> t = std::chrono::duration_cast<std::chrono::duration<double>>(t1 - t0);
		printf("Take%d Duration: %fms Total objects: %d\n", n, t.count() * 1000, n * nObjectsPerTake);
	}




	printf("Starting Read Test:\n");

	high_resolution_clock::time_point t0 = high_resolution_clock::now();
	unsigned int start = 0;
	for (unsigned int i = 0; i < nTakes * nObjectsPerTake; ++i)
	{
		STest* pInstance = myPool.GetNextUsedObject(start);
	}

	high_resolution_clock::time_point t1 = high_resolution_clock::now();
	std::chrono::duration<double> t = std::chrono::duration_cast<std::chrono::duration<double>>(t1 - t0);
	printf("Duration: %fms\n", t.count() * 1000);



	unsigned int nRandomReleaseObjects = 3 * nObjectsPerTake;
	STest** pRandomObjects = new STest*[nRandomReleaseObjects];
	printf("Collecting random objects for random releasing test...\n");

	// collect objects:
	srand(time(0));
	for (unsigned int i = 0; i < nRandomReleaseObjects; ++i)
	{
		pRandomObjects[i] = 0;
		while (!pRandomObjects[i])
		{
			unsigned int iRndObj = rand() % (myPool.GetUsedObjectCount() + myPool.GetFreeCount());
			pRandomObjects[i] = myPool.GetNextUsedObject(iRndObj);
		}
	}

	printf("Releasing randomly collected objects...\n");
	t0 = high_resolution_clock::now();
	for (unsigned int i = 0; i < nRandomReleaseObjects; ++i)
	{
		myPool.Release(&pRandomObjects[i]);
	}
	t1 = high_resolution_clock::now();
	t = std::chrono::duration_cast<std::chrono::duration<double>>(t1 - t0);
	printf("Duration: %fms\n", t.count() * 1000);




	printf("Starting Read Test Again:\n");

	t0 = high_resolution_clock::now();
	start = 0;
	for (unsigned int i = 0; i < nTakes * nObjectsPerTake; ++i)
	{
		STest* pInstance = myPool.GetNextUsedObject(start);
	}

	t1 = high_resolution_clock::now();
	t = std::chrono::duration_cast<std::chrono::duration<double>>(t1 - t0);
	printf("Duration: %fms\n", t.count() * 1000);



	printf("Running Clear() ...\n");
	t0 = high_resolution_clock::now();
	myPool.Clear();
	t1 = high_resolution_clock::now();
	t = std::chrono::duration_cast<std::chrono::duration<double>>(t1 - t0);
	printf("Duration: %fms\n", t.count() * 1000);
}