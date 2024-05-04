FLAGS := -lGL -lGLU -lglut -lm

%:
	@mkdir -p bin
	@g++ src/$@.cpp -o bin/$@ $(FLAGS)
	@./bin/$@