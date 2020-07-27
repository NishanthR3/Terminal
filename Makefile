build: src/ls.c src/fetch.c src/execute.c src/shell.c src/pinfo.c src/jobs.c src/builtin.c
	@echo "Build shell."
	gcc -o src/shell src/ls.c src/fetch.c src/execute.c src/shell.c src/pinfo.c src/jobs.c src/builtin.c

run:
	@echo "Running shell."
	src/shell

clean: 
	@echo "Cleaned."
	rm src/shell

docker_build:
	@echo "Build shell in docker."
	docker build -t terminal:latest .

docker_run:
	@echo "Run shell in docker."
	docker run -it terminal:latest

docker_clean:
	@echo "Clean docker images."
	docker rmi terminal:latest