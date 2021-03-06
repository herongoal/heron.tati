TARGET=libheron_tati.a
OBJS = heron_buffer.o heron_queue.o heron_logger.o heron_thread.o heron_worker.o \
       heron_routine.o heron_channel.o heron_pool.o heron_protocol.o heron_define.o heron_timer.o \
       heron_process.o heron_factory.o heron_engine.o \
       heron_network.o \


CC=g++
CC_FLAGS:=-std=c++14 -Wall -g -D_REENTRANT

LIB_PATH=-lpthread

all:$(TARGET)

%.o:%.cc
	$(CC) $(CC_FLAGS) -c $< -o $@ $(LIB_PATH)

$(TARGET):$(OBJS)
	rm -f $@
	ar cr $@ $(OBJS)
	rm -f $(OBJS)

clean:
	rm -f *.h.gch
	rm -f $(OBJS) $(TARGET)
