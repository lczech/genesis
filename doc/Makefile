# --------------------------------------------------------------------------------------------------
#     This makefile wraps around doxygen for easily creating the API documentation.
# --------------------------------------------------------------------------------------------------

.PHONY: api clean

# Make Doxygen API doc
api:
	@doxygen Doxyfile
	@cd latex && $(MAKE)
	@ln -s html/index.html index.html
	@ln -s latex/refman.pdf api.pdf

# Remove all generated files
clean:
	-@$(RM) -r html latex xml
	-@$(RM) index.html api.pdf
