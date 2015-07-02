.PHONY: push build build-image

DOCKER_TAG = lrde/vcsn-debuild

build-image: Dockerfile
	docker build -t $(DOCKER_TAG) .

build: build-image
	cd .. && docker run -v $$PWD:/build/vcsn $(DOCKER_TAG) /build/build.sh

push:
	docker push $(DOCKER_TAG)

