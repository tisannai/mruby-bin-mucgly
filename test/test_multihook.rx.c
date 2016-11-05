-<+Mucgly.multihook( [ "/*..", "..*/", "_L/*", "_J*/" ] )>-
/*..+@name="hookpair"..*/
/*..+/Use the non-valid hookend within next macro for testing
that is does not cause panic..*/
typedef struct _L/*Mucgly.write(@name); %Q{..*/} _J*/_s {
  gchar* beg;                   /**< Hookbeg for input file. */
  gchar* end;                   /**< Hookend for input file. */
} hookpair_t;
/*..+:block..*/

//_J*/

typedef struct filestack_s {
  GList* file;           /**< Stack of files. */
} filestack_t;
/*..+:unblock..*/
/*..+:block..*/
