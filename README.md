# FountainDB

## Supported types

In the current iteration, we only store rows with attributes id (unsigned integers) and name (strings) in-memory, in a single table.

## Grammar

In the current iteration, we support 4 kinds of statements.

To quit the repl:

```
fountainDB> .exit
```

To insert a rows into the table:

```
fountainDB> INSERT id str
```

To show all rows in the table:

```
fountainDB> SHOW
```

~~To delete a string from the table:~~

~~```~~
~~fountainDB> DELETE str~~
~~```~~

## Credits

For the initial database design, we took reference from Connor Stack's [sqlite clone in C](https://github.com/cstack/db_tutorial).
