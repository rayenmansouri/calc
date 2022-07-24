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
		hashval = hashval * 31 + *s;
	return hashval % HASHSIZE;
}

/*lookup: look for s in hashtab*/
struct nlist *lookup(char *s)
{
	struct nlist *p;
	for(p = hashtab[hash(s)];p != NULL;p =p->next)
		if(strcmp(p->name,s) == 0)
			return p;
	return NULL;
}

/*install: install (name,def) in hashtab*/
struct nlist* install(char *name,char *defn)
{
	struct nlist *np;
	unsigned hashval;
	if((np = lookup(name)) == NULL){
		np = (struct nlist *) malloc(sizeof(np));
		if(np == NULL || (np->name = strdup(name)) == NULL)
			return NULL;
		hashval = hash(name);
		np->next = hashtab[hashval]; 
		hashtab[hashval] = np;
	} else
		free((void *) np->defn);
	if((np->defn = strdup(defn)) == NULL)
		return NULL;
	return np;
}

