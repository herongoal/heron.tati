TARGET=libheron_engine.a
OBJS = heron_buffer.o heron_channel.o heron_pool.o heron_define.o heron_timer.o \
       heron_logger.o heron_process.o heron_worker.o heron_engine.o \
       heron_network.o heron_main.o \


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
