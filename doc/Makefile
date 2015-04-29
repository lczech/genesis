# --------------------------------
#   File lists
# --------------------------------

DEVTMP := dev/*.aux dev/*.glo dev/*.idx dev/*.log dev/*.toc dev/*.ist dev/*.acn dev/*.acr \
          dev/*.alg dev/*.bbl dev/*.blg dev/*.dvi dev/*.glg dev/*.gls dev/*.ilg dev/*.ind \
          dev/*.lof dev/*.lot dev/*.maf dev/*.mtc dev/*.mtc1 dev/*.out

# --------------------------------
#   Make rules
# --------------------------------

.PHONY: all clean api dev man

# Make all three docs
all: clean api dev man
	@echo "\n========== Done building =========="

# Make API doc
api:
	@echo "\n========== Build Api Doc =========="
	@cd api && doxygen Doxyfile
	@cd api/latex && $(MAKE)
	@ln -s api/html/index.html doc-api.html
	@ln -s api/latex/refman.pdf doc-api.pdf

# Make developer doc
dev:
	@echo "\n========== Build Dev Doc =========="
	@cd dev && pdflatex doc-dev
	@ln -s dev/doc-dev.pdf doc-dev.pdf

# Make manual
man:
	@echo "\n========== Build Man Doc =========="

# Remove all generated files
clean:
	@echo "\n========== Cleaning      =========="
	-@$(RM) -r api/html api/latex api/xml
	-@$(RM) $(DEVTMP) dev/*.pdf
	-@$(RM) doc-api.html doc-api.pdf doc-dev.pdf doc-man.pdf
