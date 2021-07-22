all: Ensemble Linear Voter

SRC_DIR = src
INCLUDE_DIR = include

Ensemble: $(SRC_DIR)/EnsembleClassifier.cpp $(SRC_DIR)/utils.cpp $(INCLUDE_DIR)/headers.hpp
	g++ -I include -std=c++11 $(SRC_DIR)/EnsembleClassifier.cpp $(SRC_DIR)/utils.cpp -o EnsembleClassifier.out
	
Linear: $(SRC_DIR)/LinearClassifier.cpp $(INCLUDE_DIR)/headers.hpp $(SRC_DIR)/utils.cpp
	g++ -I include -std=c++11 $(SRC_DIR)/LinearClassifier.cpp $(SRC_DIR)/utils.cpp -o Linear.out

Voter: $(SRC_DIR)/Voter.cpp $(INCLUDE_DIR)/headers.hpp $(SRC_DIR)/utils.cpp
	g++ -I include -std=c++11 $(SRC_DIR)/Voter.cpp $(SRC_DIR)/utils.cpp -o Voter.out

clean:
	rm *.out