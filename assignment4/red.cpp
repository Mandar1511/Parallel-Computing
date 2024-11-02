#include <mpi.h>
#include <iostream>
#include <cmath>
#include <vector>
#include <unistd.h> // For gethostname
#include <cassert>
#include <stdint.h>
using namespace std;

MPI_Request *send_reqs, *recv_reqs;
uint64_t *temp_recvs;

int PrintHost(int me)
{
	const size_t bufferSize = 1024; // Adjust buffer size as necessary
	std::vector<char> hostname(bufferSize);

	if (gethostname(hostname.data(), bufferSize) == 0)
	{
		std::cout << "RANK: " << me << " Hostname: " << hostname.data() << std::endl;
	}
	else
	{
		std::cerr << "Error retrieving hostname." << std::endl;
	}

	return 0;
}

inline void CheckResult(int resultCode, const char *errorMessage)
{
	if (resultCode != MPI_SUCCESS)
	{
		std::cerr << "MPI Error: " << errorMessage << std::endl;
		MPI_Abort(MPI_COMM_WORLD, resultCode);
	}
}

int AllReduce(uint64_t *sendbuf, uint64_t *recvbuf, MPI_Comm comm)
{
	int size, rank, resultCode;
	MPI_Comm_size(comm, &size);
	CheckResult(resultCode, "MPI_Comm_size failed");

	MPI_Comm_rank(comm, &rank);
	CheckResult(resultCode, "MPI_Comm_rank failed");

	int logP = (int)log2(size);

	for (int phase = 0; phase < logP; ++phase)
	{
		int msg_sender = (size + rank - (1 << phase)) % size;
		int msg_receiver = (rank + (1 << phase)) % size;
		resultCode = MPI_Irecv(&temp_recvs[phase], 1, MPI_UINT64_T, msg_sender, 0, comm, &recv_reqs[phase]);
		CheckResult(resultCode, "MPI_Comm_Irecv failed");
		resultCode = MPI_Isend(sendbuf, 1, MPI_UINT64_T, msg_receiver, 0, comm, &send_reqs[phase]);
		CheckResult(resultCode, "MPI_Comm_Isend failed");
		MPI_Wait(&recv_reqs[phase], MPI_STATUS_IGNORE);
		*sendbuf += temp_recvs[phase];
	}
	*recvbuf = *sendbuf;
	return MPI_SUCCESS;
}

double MeasureCustomImplTime(MPI_Comm comm, uint64_t iterations)
{
	int rank, size, resultCode;
	resultCode = MPI_Comm_rank(comm, &rank);
	CheckResult(resultCode, "MPI_Comm_rank failed");

	resultCode = MPI_Comm_size(comm, &size);
	CheckResult(resultCode, "MPI_Comm_size failed");
	double startTime = MPI_Wtime();
	for (uint64_t i = 0; i < iterations; ++i)
	{
		uint64_t recvbuf = 0;
		uint64_t sendbuf = i * rank;
		resultCode = AllReduce(&sendbuf, &recvbuf, comm);
		CheckResult(resultCode, "Custom AllReduce failed");
		uint64_t expect = i * (static_cast<uint64_t>(size) * (size - 1) / 2);
		if (((rank == 0) && (recvbuf != expect)) || ((rank == size - 1) && (recvbuf != expect)))
		{
			std::cout << "\n i = " << i << " expect = " << expect << " sum = " << recvbuf << "\n";
			MPI_Abort(MPI_COMM_WORLD, -1);
		}
	}
	double endTime = MPI_Wtime();
	return endTime - startTime;
}

double MeasureMPIAllreduce(MPI_Comm comm, uint64_t iterations)
{
	int rank, size, resultCode;
	resultCode = MPI_Comm_rank(comm, &rank);
	CheckResult(resultCode, "MPI_Comm_rank failed");

	resultCode = MPI_Comm_size(comm, &size);
	CheckResult(resultCode, "MPI_Comm_size failed");
	double startTime = MPI_Wtime();
	for (uint64_t i = 0; i < iterations; ++i)
	{
		uint64_t sendbuf = i * rank;
		uint64_t recvbuf = 0;
		MPI_Allreduce(&sendbuf, &recvbuf, 1, MPI_UINT64_T, MPI_SUM, comm);
		uint64_t expect = i * (static_cast<uint64_t>(size) * (size - 1) / 2);
		if (((rank == 0) && (recvbuf != expect)) || ((rank == size - 1) && (recvbuf != expect)))
		{
			std::cout << "\n i = " << i << " expect = " << expect << " sum = " << recvbuf << "\n";
			assert(recvbuf == expect);
		}
	}
	double endTime = MPI_Wtime();
	return endTime - startTime;
}

void Init(int logP)
{
	recv_reqs = new MPI_Request[logP];
	send_reqs = new MPI_Request[logP];
	temp_recvs = new uint64_t[logP];
}

int main(int argc, char **argv)
{
	MPI_Init(&argc, &argv);

	int size, rank;
	MPI_Comm_size(MPI_COMM_WORLD, &size);
	MPI_Comm_rank(MPI_COMM_WORLD, &rank);
	// PrintHost(rank);
	int logP = (int)log2(size);
	Init(logP);
	uint64_t iterations = 1000000; // Number of invocations for timing

	double builtInImplTime = MeasureMPIAllreduce(MPI_COMM_WORLD, iterations);
	double customImplTime = MeasureCustomImplTime(MPI_COMM_WORLD, iterations);

	if (rank == 0)
	{
		std::cout << "\n RANKS=" << size << " Iters=" << iterations << "\n";
		std::cout << "Custom Allreduce Time: " << customImplTime << " seconds, Rate: " << iterations / customImplTime << " allreduces/sec" << std::endl;
		std::cout << "MPI_Allreduce Time: " << builtInImplTime << " seconds, Rate: " << iterations / builtInImplTime << " allreduces/sec" << std::endl;
	}

	MPI_Finalize();
	return 0;
}