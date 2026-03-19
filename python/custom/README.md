### Custom bindings and template instances.

For some more complex functionality, we want custom bindings, wrappers,
as well as template instances that are not directly used in genesis itself.
Those are provided here.

In order to keep the generated bindings in a synchronous directory structure,
we are using the same relative paths here as we have in the main genesis library.
To avoid clashes with the includes though, we here have the convention
that all `cpp` and `hpp` files here need to have particular suffixes, such that
they cannot be accidentally confused with the original library paths.
Bit of a hacky workaround, but good enough for now.

We are currently using the following suffixes:

 - `_add_ons`: For code to be used with `add_on_binder` and `add_on_binder_for_namespace` in Binder. This typically is a header that is included via `include_for_class` and `include_for_namespace` in the Binder config, containing a function for the add on, and a source file to implement that add on binding function.
 - `_instances`: For class and function templates in genesis, to request specialization for certain types, so that Binder can generate bindings for these. This is typically a header, and then needs to be listed in `template_instances.hpp` here, so that it can be included in the auto-generated `all_includes.hpp` prior to the processing with LLVM within Binder.
 - `_wrappers`: For code that wraps existing genesis code into some form that is usable for bindings. Typically, this is a header, which then needs to be listed in `custom_wrappers.hpp` here, again so that it can be added to the `all_includes.hpp` file automatically.
