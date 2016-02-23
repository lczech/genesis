# --------------------------------------------------------------------------------------------------
#     This makefile wraps around doxygen for easily creating the API documentation.
# --------------------------------------------------------------------------------------------------

.PHONY: api clean

# Make Doxygen API doc
api:
	@cd doxygen && doxygen Doxyfile
	@ln -fs html/index.html index.html

# Remove all generated files
clean:
	-@$(RM) -r html
	-@$(RM) index.html
