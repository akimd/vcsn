.PHONY: push build-debian build-ubuntu build-debian-image build-ubuntu-image

DOCKER_DEBIAN_TAG = lrde/vcsn-debian-debuild
DOCKER_UBUNTU_TAG = lrde/vcsn-ubuntu-debuild

build-debian-image: Dockerfile
	docker build -t $(DOCKER_DEBIAN_TAG) .

build-ubuntu-image: Dockerfile.ubuntu
	docker build -t $(DOCKER_UBUNTU_TAG) -f Dockerfile.ubuntu .

build-debian: build-debian-image
	test -d ../_package || mkdir ../_package
	cd .. && docker run -v $$PWD:/build/vcsn $(DOCKER_DEBIAN_TAG) /build/build.sh

build-ubuntu: build-ubuntu-image
	test -d ../_package || mkdir ../_package
	cd .. && docker run -v $$PWD:/build/vcsn $(DOCKER_UBUNTU_TAG) /build/build.sh

push:
	docker push $(DOCKER_DEBIAN_TAG)

