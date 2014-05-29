# Makefile --- create/manage the VCSN virtual machine for distribution

all: VM/vcsn.box

unstable64.box:
	@test -f $@ || $(error "$@: file does not exist")

vcsn-bootstrap: vcsn-bootstrap.sh unstable64.box
	vagrant up --provision
	vagrant halt

.PHONY: VM/vcsn.box
VM/vcsn.box: vcsn-bootstrap
	rm -fv VM/vcsn.box
	vagrant package --output VM/vcsn.box

clean:
	vagrant halt -f
	vagrant destroy -f

	cd VM; vagrant halt -f
	cd VM; vagrant destroy -f
	rm -fv VM/vcsn.box
