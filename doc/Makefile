# --------------------------------------------------------------------------------------------------
#     This makefile wraps around doxygen for easily creating the API documentation.
# --------------------------------------------------------------------------------------------------

.PHONY: doc clean

# Make Doxygen documentation
doc:
	@cd doxygen && doxygen Doxyfile
	@ln -fs ./doc/index.html index.html

# Remove all generated files
clean:
	-@$(RM) -r ./doc
	-@$(RM) ./index.html
