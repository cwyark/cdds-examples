FROM ubuntu:16.04

RUN apt-get update && apt-get install -y cmake openjdk-8-jdk  gcc