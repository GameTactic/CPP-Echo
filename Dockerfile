FROM alpine:latest

WORKDIR "/compile"

COPY . /compile

RUN apk update && apk add build-base unzip wget cmake libstdc++ boost-dev git && \
        wget https://github.com/zaphoyd/websocketpp/archive/master.zip && \
	unzip master.zip && rm master.zip && \
        cd websocketpp-master && \
        cmake . && make -j $(nproc --all) && make install && \
	cd .. && rm -rf websocketpp-master && \
	cmake . && make -j $(nproc --all) && make install && \
        mv bin/server /bin/server && \
	apk --purge del build-base unzip wget cmake git boost-dev && \
	rm -rf /cache/apk/* && cd / && rm -rf /compile

WORKDIR "/"
ENTRYPOINT "/bin/server"
EXPOSE 80/tcp
