FROM alpine:3.10

WORKDIR "/compile"

COPY . /compile

RUN apk update && apk add build-base cmake libstdc++ boost-dev git && \
	git submodule update --init --recursive && \
	cmake . && make -j $(nproc --all) && make install && \
        mv bin/server /bin/server && \
	apk --purge del build-base unzip wget cmake git boost-dev && \
	rm -rf /cache/apk/* && cd / && rm -rf /compile

WORKDIR "/"
ENTRYPOINT "/bin/server"
EXPOSE 80/tcp
