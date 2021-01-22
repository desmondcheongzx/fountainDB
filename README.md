# FountainDB

## Supported types

In the current iteration, we support persistent storage of a single table on disk. However, we fix rows to have two attributes: id (unsigned integers), and name (strings).

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

For the initial database design, we took reference from Connor Stack's [sqlite clone in C](https://github.com/cstack/db_tutorial) under [The MIT License](https://opensource.org/licenses/MIT).
