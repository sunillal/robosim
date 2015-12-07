#Sunil Lal
#23-AUG-2007

CC = g++ -Wall -g -O0
TARGET = roboSim
SOURCE = main.cpp DEEngine.cpp GAEngine.cpp Chromosome.cpp mt19937ar.cpp
HEADERS = Simulator.h Controller.h FSMController.h CAController.h NNController.h MotionEvaluator.h GAMotionOptimizer.h GAEngine.h Chromosome.h DEEngine.h DEMotionOptimizer.h mt19937ar.h constants.h
OBJS   = $(SOURCE:.cpp=.o)
LIBS =  -L../drawstuff/src/.libs -L/usr/lib -L/usr/local/lib 
INDS = -I. -I../include -I/usr/include -I/usr/include/ode
OPTS =  -lm -lode -ldrawstuff -lstdc++ -lpthread -DdDOUBLE
GL_LIBS = -framework OpenGL -framework GLUT

all:$(SOURCE) $(TARGET)

$(TARGET):$(OBJS)
	$(CC) -o $(TARGET) $(OBJS) $(LIBS) $(INDS) $(OPTS) $(GL_LIBS)

.cpp.o:
	$(CC) -c  $(LIBS) $(INDS) $< -o $@

$(OBJS):$(HEADERS)

clean:
	rm $(TARGET) $(OBJS)
