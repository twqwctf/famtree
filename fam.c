
/**********************************************************************************

 Program 3: fam.c	"The family tree program"
 Due date: 11-29-89	(Turned in 12-1-89 - USING 2 EXTENSION DAYS)
 Programmer: Greg Fudala
 SS: xxx-xx-xxxx
 System: MAC under UNIX
 Compiler: cc
 Purpose: This program allows the user to build, maintain, and retrieve information
	  from a family tree structure. The executable image takes two arguments-
	  a.out <infile> <outfile> the first being the input file, the second being 
	  the output file. The program reads commands and arguments (if applicable)
	  from the input file and performs the appropriate structure manipulation
	  or family information retrival from the family tree structure. The command
	  specifics (format and rules) are explained in the assignment sheet.
	     Syntactic errors in the input file will be detected and the appropriate
          error message will be outputed. All output is sent to <outfile>. 

	  NOTE: -The optional 'define' command was not implemented - the only relations
	         are the three 'primative' ones - parent, spouse, & child.
		-Regardless, the input line is echoed out if an error (syntactic or 
		 logical) is detected.

 **********************************************************************************/

#include <stdio.h>;				/* external standard i/o routines */

FILE *infile, *outfile,				/* pointers to input and output files */ 
     *fopen();					/* use of external open FILE routine */ 

struct person {					/* type for each person (node) in the */
   char name[20];				/* family tree structure */
   char sex;
   struct person *mother;
   struct person *father;
   struct person *nextsib;
   struct person *spouse;
   struct person *firstchild;
   };

struct rootlist {				/* type for the header nodes of the */
   struct person *root;				/* family tree structure */
   struct rootlist *next;
   };

typedef struct rootlist *ptrcreations;		/* typedef ptr to struct rootlist */
typedef struct person *ptrperson;		/* typedef ptr to struct person */

struct rootlist *creations,   			/* variable ptr to head nodes */ 
		*firstcreations, 		/* assigned to initial creations ptr */
		*tempcreations;			/* updated ptr to creations */

char buffer[80];				/* input line string from <infile> */
char command[7],				/* command string */
     name1[20], name2[20],			/* name strings */
     sex,					/* sex ("M" or "F") */
     relation[7];				/* relation string */ 

struct person *fam,				/* ptr to a node in family tree */
	      *per1, *per2;			/* access ptrs to a node in the tree */

nullperson(ptr)

/********************************************************************************

 function nullperson
 Purpose: This function takes a pointer to a person node in the tree and sets its
	  pointers of the same type as itself to NULL.

 ********************************************************************************/

struct person *ptr;
   {
   ptr->mother=NULL;
   ptr->father=NULL;
   ptr->nextsib=NULL;
   ptr->spouse=NULL;
   ptr->firstchild=NULL;
   }

error(index) 

/********************************************************************************

 function error
 Purpose: This function takes an index and outputs the corresponding error message
	  while echoing the input line.

 ********************************************************************************/

int index;
   {
   fprintf(outfile, "%s\n", buffer);
   switch(index) {
   case 5:fprintf(outfile, "QUESTION: Error- person not found\n"); break;
   case 1:fprintf(outfile, "BEGET: error- child already exists\n"); break;
   case 2:fprintf(outfile, "DISPLAY: Error- name does not exist\n"); break;
   case 3:fprintf(outfile, "BEGET: error- mother does not exist\n"); break;
   case 4:fprintf(outfile, "BEGET: error- mother not female\n"); break;
   case 6:fprintf(outfile, "BEGET: error- mother not married\n"); break;
   case 7:fprintf(outfile, "MARRY: Error- person not found\n"); break;
   case 8:fprintf(outfile, "CREATE: Error- duplicate creation requested\n"); break;
   case 9:fprintf(outfile, "MARRY: Error- gay marriage\n"); break;
   case 10:fprintf(outfile,"MARRY: Error- attempted bigamy\n"); break;
   case 11:fprintf(outfile, "ERROR IN LINE\n"); break;
   case 12:fprintf(outfile, "QUERY: Error- person not found\n"); break;
   case 13:fprintf(outfile, "DISOWN: Error- person not found\n"); break;
   }
   fprintf(outfile, "---------------------\n");
   }
   
putinlist(creations,fam)

/**********************************************************************************

 function putinlist
 Purpose: This function takes a ptr to a person (fam) and the address of what creations
	  is pointing to (called with &creations). It then finds the next available
	  header node (searches from initial header node on) and assigns its root
	  field to the person (fam).

 **********************************************************************************/

ptrcreations *creations;
struct person *fam;
   {
   if (*creations==NULL)
      {
      *creations=(struct rootlist *)malloc(sizeof(struct rootlist));
      if (firstcreations==NULL)
         firstcreations=(*creations);
      (*creations)->next=NULL;
      (*creations)->root=fam;
      }
   else
      putinlist(&((*creations)->next),fam);
   }
 
getname(fam,per,name)

/**********************************************************************************

 function getname
 Purpose: This function, given the name of a person, searches the children of a header 
	  node and returns(by parameter) a ptr to the person if the person is found.

 **********************************************************************************/

struct person *fam;
ptrperson *per;
char name[];
   {
   if (fam!=NULL)
      {
      getname(fam->firstchild,per,name);
      if (strcmp(fam->name,name) == 0)
         (*per)=fam;
      getname(fam->nextsib,per,name);
      }
   }

struct person *searchandget(name)

/**********************************************************************************

 function searchandget
 Purpose: This function searches the header nodes starting with the first and calls
	  getname searching for a person. If the person is found, the function returns
	  the pointer to the person, else- it returns NULL.

 **********************************************************************************/

char name[];
   {
   struct person *per;
   per=NULL;
   tempcreations=firstcreations;
   while (tempcreations!=NULL)
      {  
      getname(tempcreations->root,&per,name);
      tempcreations=tempcreations->next;
      }
   return(per); 
   }
   
create(name1,sex)

/*********************************************************************************

 function create
 Purpose: This function, given a name and sex, allocates memory for a new person node
	  and inserts the person in the next available header node (allocates memory
	  for a new header node also(done in putinlist)).

 *********************************************************************************/

char name1[],sex;
   {
   per1=NULL;
   per1 = searchandget(name1);
   if ((per1!=NULL) && (per1->sex==sex))		/* duplicate person */
      error(8);
   else
   {
   fam=(struct person *)malloc(sizeof(struct person));
   nullperson(fam);
   strcpy(fam->name,name1);
   fam->sex=sex;
   putinlist(&creations,fam);
   }
   } 

marry(name1,name2)

/*********************************************************************************

 function marry
 Purpose: This function marries two people by setting their spouse pointers to each
	  other.

 *********************************************************************************/

char name1[],name2[];
   {
   per1 = NULL;
   per2 = NULL;
   per1 = searchandget(name1);
   per2 = searchandget(name2);
   if ((per1==NULL) || (per2==NULL))		/* one or both persons not found */
      error(7);
   else
   if (per1->sex==per2->sex)			/* gay marriage */
      error(9);
   else
   if ((per1->spouse != NULL) || (per2->spouse != NULL))   /* already married */
      error(10);
   else
   {
   per1->spouse=per2;
   per2->spouse=per1;
   }
   }

addnextsib(sib,newchild)

/*********************************************************************************

 function addnextsib
 Purpose: This function takes a mother's firstchild and searches its nextsiblings
	  recursively until NULL. It then assigns to that pointers nextsib field
	  the firstchild.

 *********************************************************************************/

struct person *sib, *newchild;
   {
   if (sib->nextsib != NULL)
      addnextsib(sib->nextsib, newchild);
   else
      sib->nextsib = newchild;
   }

beget(name1,sex,name2)

/*********************************************************************************

 function beget
 Purpose: This function adds a new child (name1) to the mother. If the mother has
	  no firstchild, the child is assigned to the mothers firstchild field. Else,
	  addnextsib must be called to find the mothers youngest child to assign
	  to its nextsib field.

 ********************************************************************************/

char name1[], sex, name2[];
   {
   per1 = NULL;
   per1 = searchandget(name1);
   if (per1 != NULL)
      error(1);
   else
   {
   per2 = NULL;
   per2 = searchandget(name2);
   if (per2 == NULL)			/* mother not found */
      error(3);
   else
   if (per2->sex != 'F')		/* mother not female */
      error(4);
   else
   if (per2->spouse == NULL)		/* mother already married */
      error(6);
   else		/* BEGET */
   {
   per1=(struct person *)malloc(sizeof(struct person)); 
   strcpy(per1->name,name1);
   per1->sex = sex;
   nullperson(per1);
   per1->mother = per2;			/* set childs mother & father fields */
   per1->father = per2->spouse;
   if (per2->firstchild == NULL)
      {
      per2->firstchild = per1;		/* set mother & fathers firstchild to child */
      per2->spouse->firstchild = per1;
      }
   else
      addnextsib(per2->firstchild,per1);
   }
   }
   }

display(per,indent)

/*********************************************************************************

 function display
 Purpose: This function given initially the firstchild pointer of the requested
	  person to display will recursively print the persons children and their
	  spouses (if one) and their children etc.... 

 *********************************************************************************/

struct person *per;
int indent;
   {
   int i;
   if (per != NULL)
      {
      if (per->firstchild == NULL)	 	/* not parent */
         {
         for (i=1; i<indent; ++i)
            fprintf(outfile, "%c", 32);
         fprintf(outfile, "%s", per->name);
         if (per->spouse != NULL)
            fprintf(outfile, "%c %s\n", 32, per->spouse->name);
         else
            fprintf(outfile, "\n");
         }
      else					/* a parent */
         {
         for (i=1; i<indent; ++i)
            fprintf(outfile, "%c", 32);
         fprintf(outfile, "%s", per->name);
         fprintf(outfile, "%c %s\n", 32, per->spouse->name);
         display(per->firstchild, indent+6);
         }
         display(per->nextsib, indent);
      }
   }

question(name1, relation, name2)

/*********************************************************************************

 function question
 Purpose: This function, if name1 is a relation of name2, prints "Yes" - else "No".

 *********************************************************************************/

char name1[], relation[], name2[];
   {
   per1=NULL;
   per2=NULL;
   per1 = searchandget(name1);
   per2 = searchandget(name2);
   if ((per1 == NULL) || (per2 == NULL))		/* person not found */
      error(5);
   else
      {
      fprintf(outfile, "\n %s\n", buffer);
      if (strcmp(relation, "parent")==0)
       	 if ((per2->mother == per1) || (per2->father == per1))
	    fprintf(outfile, "Yes\n");
	 else
	    fprintf(outfile, "No\n");
      else
      if (strcmp(relation, "spouse")==0)
	 if (per2->spouse == per1)
	    fprintf(outfile, "Yes\n");
         else
	    fprintf(outfile, "No\n");
      else
      if (strcmp(relation, "child")==0)
	 if ((per1->mother == per2) || (per1->father == per2)) 
	    fprintf(outfile, "Yes\n");
         else
	    fprintf(outfile, "No\n");
      else
	 error(11);
      }
      }

printchildren(per)

/**********************************************************************************

 function printchildren 
 Purpose: This function given a persons firstchild will print the firstchild and all
	  following siblings.

 **********************************************************************************/

struct person *per;
   {
   if (per != NULL)
      {
      fprintf(outfile, "%s\n", per->name);
      printchildren(per->nextsib);
      }
   }

query(relation, name)

/**********************************************************************************

 function query
 Purpose: This function given a name and a relation prints all persons who qualify
	  as a relation of name.

 **********************************************************************************/

char relation[], name[];
   {
   per1=NULL;
   per1 = searchandget(name);
   if (per1 == NULL)
      error(12);
   else
      {
      fprintf(outfile, "\n %s\n", buffer);
      if (strcmp(relation, "parent")==0)
	 {
	 if (per1->mother != NULL)
	    {
	    fprintf(outfile, "%s\n", per1->mother->name);
	    fprintf(outfile, "%s\n", per1->father->name);
	    }
         }
      else
      if (strcmp(relation, "spouse")==0)
	 {
	 if (per1->spouse != NULL)
	    fprintf(outfile, "%s\n", per1->spouse->name);
         }
      else
      if (strcmp(relation, "child")==0)
	 { 
	 if (per1->firstchild != NULL)		/* TRUE- 1 or greater children */
	    printchildren(per1->firstchild);
         }
      else
	 error(11);
      }
   }

subtreegetlink(fam,per,name)

/***********************************************************************************

 function subtreegetlink
 Purpose: This function, given a pointer to a header nodes root field and a person to
	  search for will return(by parameter) the persons link - that is - its
	  older brother or sister. This is so that, when the person is disowned, 
	  if the person is in a nextsib link, the persons upper link nextsibling will
	  now be set to the persons nextsibling to skip the person as if he did not
	  exist.
	   
 ***********************************************************************************/

struct person *fam;
ptrperson *per;
char name[];
   {
   if (fam != NULL)
      {
      subtreegetlink(fam->firstchild,per,name);
      if (strcmp(fam->nextsib->name,name) == 0)    /* current ptr nextsib = person */
	 (*per) = fam;
      subtreegetlink(fam->nextsib,per,name);
      }
   }
      

struct person *getlink(name)

/***********************************************************************************

 function getlink
 Purpose: This function starts at the first header node and searches all header nodes'
	  root fields searching for name's upper link (as explained in subtreegetlink).

 ***********************************************************************************/

char name[];
   {
   struct person *per;
   per=NULL;
   tempcreations = firstcreations;
   while (tempcreations != NULL)
      {
      subtreegetlink(tempcreations->root, &per, name);
      tempcreations = tempcreations->next;
      }
   return(per);
   }

disown(per,name)

/***********************************************************************************

 function disown
 Purpose: This function disowns a person (name). All children below the person must
	  be deleted. Recursively, 
	  if the node has a spouse, 
	     the nodes spouses spouse and the nodes spouses firstchild is set to NULL.
	  If the node is the initial deletion person,
	     if the node has a mother, and the node is a firstchild,
	     the nodes mothers firstchild is now set to the nodes nextsib. The same is
	     done for the nodes father. 
          if the node is not the deletion person,
	     if the nodes has a mother
		the mothers firstchild is set to NULL.
		the same for the fathers firstchild.

 ***********************************************************************************/
	     
struct person *per;
char name[];
   {
   struct person *link;
   if (per != NULL)
      {
      disown(per->firstchild,name);
      if (per->spouse != NULL)				/* node has spouse */
	 {
         per->spouse->spouse = NULL;
         per->spouse->firstchild = NULL;
	 }
      if (strcmp(per->name,name)==0)    		/* on deletion person */
	 {
	 if (per->mother != NULL)			/* node has mother */
	    if (strcmp(per->mother->firstchild->name,name)==0) /* node is firstchild */
	       {
	       per->mother->firstchild = per1->nextsib;
	       per->father->firstchild = per1->nextsib;
	       }
            else					/* node is not firstchild */
	       {
	       link = getlink(name);
	       link->nextsib =per1->nextsib;
	       }
         }
      else						/* not on deletion person */
	 {
         if (per->mother != NULL)			/* node has mother */
            {
            per->mother->firstchild = NULL;
	    per->father->firstchild = NULL;
	    }
	 }
      free(per);				/* free memory for this person node */
      disown(per->nextsib,name);

       }
   }

headercheck(per,creations)

/***********************************************************************************

 function headercheck
 Purpose: This function searches through the header nodes starting at the beginning
	  (firstcreations ptr) and if the person to disown is a root of the headers
	  next fields root, then the headers next field becomes its next fields next
	  field (to bypass the deletion person).

 ***********************************************************************************/

struct person *per;
struct rootlist *creations;
   {
   if (creations != NULL)
      {
      if (creations->next != NULL)
         if (strcmp(creations->next->root->name, per->name) == 0)
	    {
	    creations->next = creations->next->next;
	    free(creations);			/* free memory of this one header */
	    }
      }
   else
      headercheck(per,creations->next);
   } 

clear(creations)

/**********************************************************************************
 
 function clear
 Purpose: This function searches all header nodes starting at the beginning (called
	  with firstcreations) and performs a disown operation on the headers
	  root field. Essentially, this clears the entire family tree.

 **********************************************************************************/

struct rootlist *creations;
   {
   if (creations != NULL)
      {
      headercheck(creations->root,creations); 
      disown(creations->root,creations->root->name);
      clear(creations->next);
      }
   }


main(argc,argv)
int argc;					/* number of command line arguments */
char *argv[];					/* array of pointers to command line */
{						/*    arguments inputed by user */
infile=fopen(argv[1],"r");			/* open input file */
outfile=fopen(argv[2],"w");			/* open output file */  

creations=NULL;					
firstcreations=NULL;

fgets(buffer,80,infile);			/* read input line from infile */
while (!feof(infile))
   {
   sscanf(buffer,"%s",command);
   if (strcmp(command,"create")==0)
      if (sscanf(buffer,"%*s %s %c",name1,&sex) != 2)
	 error(11);
      else
         create(name1,sex);  
   else
   if (strcmp(command,"marry")==0)
      if (sscanf(buffer, "%*s %s %s", name1, name2) != 2)
	 error(11);
      else
         marry(name1,name2);
   else
   if (strcmp(command,"beget")==0)
      if (sscanf(buffer, "%*s %s %c %s", name1, &sex, name2) !=3)
	 error(11);
      else
         beget(name1,sex,name2);
   else
   if (strcmp(command,"display")==0)
      if (sscanf(buffer, "%*s %s", name1) != 1)
	 error(11);
      else
      {
      per1 = NULL;
      per1 = searchandget(name1);
      if (per1 == NULL)				/* person not found */
	 error(2);
      else
	 {
	 fprintf(outfile, "\n %s\n", buffer);	/* echo input line */
	 fprintf(outfile, "%s", per1->name);	/* output name */
	 if (per1->spouse != NULL)
	    fprintf(outfile,"%c %s\n", 32, per1->spouse->name);	  /* output spouse */
         else
	    fprintf(outfile, "\n");
         display(per1->firstchild,6);
	 fprintf(outfile, "\n");
	 }
      }
   else
   if (strcmp(command, "question")==0)
      if (sscanf(buffer, "%*s %s %s %s", name1, relation, name2) != 3)
	 error(11);
      else
         question(name1, relation, name2);
   else
   if (strcmp(command, "query")==0)
      if (sscanf(buffer, "%*s %s %s", relation, name1) != 2)
	 error(11);
      else
	 query(relation,name1);
   else
   if (strcmp(command, "disown")==0)
      if (sscanf(buffer, "%*s %s", name1) != 1)
	 error(11);
      else
	 {
	 per1 = NULL;
	 per1 = searchandget(name1);
	 if (per1 == NULL)				/* person not found */
	    error(13);
         else
	    {
            headercheck(per1,firstcreations);
	    disown(per1,name1); 
	    }
         }
   else
   if (strcmp(command, "clear")==0)
      {
      clear(firstcreations);
      firstcreations = NULL;
      }
   else
      error(11);
   fgets(buffer,80,infile);				/* read input from infile */
   
   }
fclose(infile);						/* close input & output files */
fclose(outfile); 
}
