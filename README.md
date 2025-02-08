# Gstreamer app

## Building

To build the Docker container:
```bash
make build
```
This command creates a Docker image containing your application.

## Running

To enter the Docker container:
```bash
make run
```
This command starts the container and provides shell access.

## Local Execution

To copy the compiled binary (`a.out`) to your host machine:
```bash
make copy
```
This allows you to run the application directly on your computer without entering the container.