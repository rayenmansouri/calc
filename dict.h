#define HASHSIZE 101

struct nlist{
	struct nlist *next;
	char *name;
	char *defn;
};

static struct nlist *hashtab[HASHSIZE];

unsigned hash(char *s)
{
    unsigned hashval;
    for(hashval = 0;*s != '\0';s++)
        hashval = 31*hashval + *s;
    return hashval % HASHSIZE;
}

/*lookup: look for s in hashtab*/
struct nlist *lookup(char *name)
{
    struct nlist *np;
    for(np = hashtab[hash(name)];np != NULL;np = np->next)
        if(strcmp(name,np->name) == 0)
            return np;
    return NULL;
}

/*install: install (name,def) in hashtab*/
struct nlist *install(char *name,char *defn)
{
    struct nlist *np;
    if((np = lookup(name)) == NULL){
        np = (struct nlist *) malloc(sizeof(*np));
        if(np == NULL || (np->name = strdup(name)) == NULL)
            return NULL;
        np->next = hashtab[hash(name)];
        hashtab[hash(name)] = np;
    }else
        free((void *) np->defn);
    if((np->defn = strdup(defn)) == NULL)
        return NULL;
    return np;
}










