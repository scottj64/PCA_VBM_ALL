CPPFLAGS=-std=c++11 -pthread#-g -Wall 

#BOOST_HOME=/home/yichen.lyh/boost_home
TNT_HOME=/home/aspelun1/Downloads/PCA_nist/tnt
JAMA_HOME=/home/aspelun1/Downloads/PCA_nist/jama

INCLUDE = -I ${TNT_HOME} -I ${JAMA_HOME}

#CPLUS_INCLUDE_PATH=${BOOST_HOME}/include
#export CPLUS_INCLUDE_PATH

.PHONY : clean all

all: $(subst .cpp,.o,$(SOURCES))  generateCalibrationMatrix


%.O: %.cpp
	$(CXX) $(CPPFLAGS) ${INCLUDE} $^ $@
generateCalibrationMatrix: generateCalibrationMatrix.cpp
	$(CXX) $(CPPFLAGS) ${INCLUDE} $^ -o $@

clean:
	rm -rf  *.o  generateCalibrationMatrix
