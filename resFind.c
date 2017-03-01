# include <stdio.h>
# include <errno.h>
# include <stdlib.h>
# include <string.h>
# include <unistd.h>
# include <pml/parser.h>
# include <pml/scanner.h>
# include "global.h"
# include "local.h"
# include "common.h"
# include "main.h"
# include "link/link.h"

extern int main (
# ifdef ANSI_PROTOTYPES
    int			/* argc */,
    String *		/* argv */
# endif
);

int main (argc, argv)
     int     argc;
     String *argv;
{
  int c;
  int status;
  filename = "-";
  status = EXIT_SUCCESS;
   Node node;
  do {
    if (optind < argc) {
      filename = argv [optind];
      lineno = 1;
    }
    
    if (strcmp (filename, "-") == 0) {
      filename = "stdin";
      yyin = stdin;
      //program is a graph defined in parser.h
      if (yyparse ( ) == 0) {
		 ReduceGraph (program);
		 for (node = graph -> source; node != NULL; node = node -> next){
	      if (node -> requires != NULL){
			GetDrugId (node -> requires, node, graph); 
			} 
		}
		GraphDestroy (program);
      } else{
		status = EXIT_FAILURE;
     } 
    } else if ((yyin = fopen (filename, "r")) != NULL) {
	      if (yyparse ( ) == 0) {
			ReduceGraph (program);
			 for (node = graph -> source; node != NULL; node = node -> next){
		      if (node -> requires != NULL){
				GetDrugId (node -> requires, node, graph); 
				}
			}
				GraphDestroy (program);
	    }else{
		status = EXIT_FAILURE;
	    }
	      fclose (yyin);
    } else {
      fprintf (stderr, "%s: ", argv [0]);
      perror (filename);
      status = EXIT_FAILURE;
    }
    
  } while (++ optind < argc);
  
  exit (status);
}

static void getDrugId(tree, node, graph) 
	 Tree  tree;
     Node  node;
     Graph graph;
{
	FILE *fp = fopen("drug_list.txt", "w+");
	if (f == NULL)
	{
	    printf("Error opening file!\n");
	    exit(1);
	}

  /* Step down the tree until we hit an expression */
  if(IS_OP_TREE (tree)) {
  	//IsResource(tree) && HasAttribute(tree)
		//uses getExpression
		  	if(IsResource(tree) && strcmp(GetResourceName(tree), "drug")==0) {
		      if(HasAttribute(tree) && strcmp(GetAttributeName(tree->right),"id")==0){
				fprintf(fp, "%s\n",GetExpression(tree));
				//fprintf(fp, "%s\n",tree-right->sval);
				}
				else{
					getDrugId(tree->right,node,graph);
				}
		    }  
		  	else{
		  		getDrugId(tree->left,node,graph);
		  	}
  		}
    // if resource found, add drug id to array  
	}
}


