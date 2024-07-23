.PHONY: compile tangle-org check-emacs check-file 

tangle-org: check-emacs check-file
	echo "Tangling org files"
	emacs -batch \
	  --eval "(progn (require 'org) (require 'ob))" \
	  --eval "(setq default-directory \"$(shell pwd)\")" \
	  --eval "(with-current-buffer (find-file-noselect \"./tcp-client-server.org\") (goto-char (point-min)) (org-babel-tangle))"

check-emacs:
	@which emacs > /dev/null && echo "Emacs found at $(shell which emacs)" || (echo "Emacs not found." && false)



check-file:
	@if [ ! -f ./tcp-client-server.org ]; then echo "File tcp-client-server.org not found."; false; fi
compile: tangle-org
	cc -o client client.c
	cc -o server server.c
