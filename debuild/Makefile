.PHONY: push build build-image

DOCKER_TAG = lrde/vcsn-debuild

build-image: Dockerfile 02proxy build.sh sources.list
	docker build -t $(DOCKER_TAG) --pull=true --no-cache --force-rm .

build:
	rm -rf ../_package
	mkdir ../_package
	cd .. && docker run --rm -e NBPROC=$$NBPROC -v $$PWD:/build/vcsn $(DOCKER_TAG) /build/build.sh

push:
	docker push $(DOCKER_TAG)
