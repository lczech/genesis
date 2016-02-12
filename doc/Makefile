# --------------------------------------------------------------------------------------------------
#     This makefile wraps around doxygen for easily creating the API documentation.
# --------------------------------------------------------------------------------------------------

.PHONY: api clean

# Make Doxygen API doc
api:
	@doxygen Doxyfile
	@ln -s html/index.html index.html

# Remove all generated files
clean:
	-@$(RM) -r html xml
	-@$(RM) index.html
