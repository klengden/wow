include makefile.inc

NOW = $(shell date +"%Y-%m-%d(%H:%M:%S %z)")

# Extra destination directories
PKGDIR = ./output/$(MACHINE)/pkg/

define create_changelog
	@$(ECHO) "Update changelog"
	mv CHANGELOG.md CHANGELOG.md.bak
	head -n 9 CHANGELOG.md.bak > CHANGELOG.md
	$(ECHO) "" >> CHANGELOG.md
	$(ECHO) "## Release $(VERSION) - $(NOW)" >> CHANGELOG.md
	$(ECHO) "" >> CHANGELOG.md
	$(GIT) log --pretty=format:"- %s" $$($(GIT) describe --tags | grep -v "merge" | cut -d'-' -f1)..HEAD  >> CHANGELOG.md
	$(ECHO) "" >> CHANGELOG.md
	tail -n +10 CHANGELOG.md.bak >> CHANGELOG.md
	rm CHANGELOG.md.bak
endef

# targets
all:
	$(MAKE) -C src all

clean:
	$(MAKE) -C src clean
	$(MAKE) -C test clean
	$(MAKE) -C doc clean

install: all
	$(INSTALL) -D -p -m 0644 odl/helloworld_defaults.odl $(DEST)/etc/amx/helloworld/helloworld_defaults.odl
	$(INSTALL) -D -p -m 0644 odl/helloworld_definition.odl $(DEST)/etc/amx/helloworld/helloworld_definition.odl
	$(INSTALL) -D -p -m 0644 odl/helloworld_extra.odl $(DEST)/etc/amx/helloworld/helloworld_extra.odl
	$(INSTALL) -D -p -m 0644 odl/helloworld.odl $(DEST)/etc/amx/helloworld/helloworld.odl
	$(INSTALL) -D -p -m 0755 odl/helloworld.odl $(DEST)/usr/bin/helloworld.odl
	$(INSTALL) -D -p -m 0644 output/$(MACHINE)/object/helloworld.so $(DEST)/usr/lib/amx/helloworld/helloworld.so
	$(INSTALL) -d -m 0755 $(DEST)$(BINDIR)
	ln -sfr $(DEST)$(BINDIR)/amxrt $(DEST)$(BINDIR)/helloworld
	$(INSTALL) -D -p -m 0755 scripts/$(COMPONENT).sh $(DEST)$(INITDIR)/$(COMPONENT)

package: all
	$(INSTALL) -D -p -m 0644 odl/helloworld_defaults.odl $(PKGDIR)/etc/amx/helloworld/helloworld_defaults.odl
	$(INSTALL) -D -p -m 0644 odl/helloworld_definition.odl $(PKGDIR)/etc/amx/helloworld/helloworld_definition.odl
	$(INSTALL) -D -p -m 0644 odl/helloworld_extra.odl $(PKGDIR)/etc/amx/helloworld/helloworld_extra.odl
	$(INSTALL) -D -p -m 0644 odl/helloworld.odl $(PKGDIR)/etc/amx/helloworld/helloworld.odl
	$(INSTALL) -D -p -m 0755 odl/helloworld.odl $(PKGDIR)/usr/bin/helloworld.odl
	$(INSTALL) -D -p -m 0644 output/$(MACHINE)/object/helloworld.so $(PKGDIR)/usr/lib/amx/helloworld/helloworld.so
	$(INSTALL) -d -m 0755 $(PKGDIR)$(BINDIR)
	ln -sfr $(PKGDIR)$(BINDIR)/amxrt $(PKGDIR)$(BINDIR)/helloworld
	$(INSTALL) -D -p -m 0755 scripts/$(COMPONENT).sh $(PKGDIR)$(INITDIR)/$(COMPONENT)
	cd $(PKGDIR) && $(TAR) -czvf ../$(COMPONENT)-$(VERSION).tar.gz .
	cp $(PKGDIR)../$(COMPONENT)-$(VERSION).tar.gz .
	make -C packages

changelog:
	$(call create_changelog)

doc:
	$(MAKE) -C doc doc

	$(eval ODLFILES += odl/helloworld_defaults.odl)
	$(eval ODLFILES += odl/helloworld_definition.odl)
	$(eval ODLFILES += odl/helloworld_extra.odl)
	$(eval ODLFILES += odl/helloworld.odl)
	$(eval ODLFILES += odl/helloworld.odl)

	mkdir -p output/xml
	mkdir -p output/html
	mkdir -p output/confluence
	amxo-cg -Gxml,output/xml/$(COMPONENT).xml $(or $(ODLFILES), "")
	amxo-xml-to -x html -o output-dir=output/html -o title="$(COMPONENT)" -o version=$(VERSION) -o sub-title="Datamodel reference" output/xml/*.xml
	amxo-xml-to -x confluence -o output-dir=output/confluence -o title="$(COMPONENT)" -o version=$(VERSION) -o sub-title="Datamodel reference" output/xml/*.xml

.PHONY: all clean changelog install package doc test
