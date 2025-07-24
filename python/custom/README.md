### Custom bindings and template instances.

For some more complex functionality, we want custom bindings,
as well as template instances that are not directly used in genesis itself.
Those are provided here.

In order to keep the generated bindings in a nice directory structure,
we trick Binder into using the same relative paths here as we have in genesis.
To avoid clashes with the includes though, we here have the convention
that all cpp and hpp files here need to end in `_bind`, so that they cannot
be accidentally confused with the original library paths.
Bit of a hacky workaround, but good enough for now.
