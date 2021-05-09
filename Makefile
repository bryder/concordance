all:
	# https://www.msys2.org/docs/filesystem-paths/
	# see also skeletons and snippets for notes
	cd concordance && env MSYS2_ENV_CONV_EXCL="*" $(MAKE) all
#	cd libconcord && $(MAKE) all
#	echo $(MAKE)
#	$(MAKE) -C concordance all
#	$(MAKE) -C libconcord all

clean:
	echo $(MAKE)
	cd concordance && $(MAKE) clean
	cd libconcord && $(MAKE) clean


