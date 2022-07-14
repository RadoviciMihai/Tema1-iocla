#include <stdio.h>

#include <stdlib.h>

#include <string.h>

typedef struct Dir {
  char * name;
  struct Dir * parent;
  struct File * head_children_files;
  struct Dir * head_children_dirs;
  struct Dir * next;
}
Dir;

typedef struct File {
  char * name;
  struct Dir * parent;
  struct File * next;
}
File;

Dir * alocdir(Dir * parent, char * name) {
  Dir * nod = malloc(sizeof(Dir));
  nod -> name = malloc(strlen(name) + 1);
  strcpy(nod -> name, name);
  nod -> next = NULL;
  nod -> head_children_dirs = NULL;
  nod -> head_children_files = NULL;
  nod -> parent = parent;
  return nod;
}

File * alocfile(Dir * parent, char * name) {
  File * nod = malloc(sizeof(File));
  nod -> name = malloc(strlen(name) + 1);
  strcpy(nod -> name, name);
  nod -> next = NULL;
  nod -> parent = parent;
  return nod;
}

void destroy_file_list(File * nod);
void destroy_dir_list(Dir * nod);
void destroy_file(File * nod);
void destroy_dir(Dir * nod);

void destroy_file_list(File * nod) {
  if (nod == NULL)
    return;
  destroy_file_list(nod -> next);
  destroy_file(nod);
}

void destroy_dir_list(Dir * nod) {
  if (nod == NULL)
    return;
  destroy_dir_list(nod -> next);
  destroy_dir(nod);
}

void destroy_dir(Dir * nod) {
  destroy_file_list(nod -> head_children_files);
  destroy_dir_list(nod -> head_children_dirs);
  free(nod -> name);
  free(nod);
  return;
}

void destroy_file(File * nod) {
  free(nod -> name);
  free(nod);
  return;
}

void touch(Dir * parent, char * name) {
  File * nod = alocfile(parent, name);
  if (parent -> head_children_files == NULL) {
    parent -> head_children_files = nod;
  } else {
    File * iter = parent -> head_children_files;
    while (iter -> next != NULL) {
      if (strcmp(iter -> name, nod -> name) == 0) {
        destroy_file(nod);
        printf("File already exists\n");
        return;
      }
      iter = iter -> next;
    }
    if (strcmp(iter -> name, nod -> name) == 0) {
      destroy_file(nod);
      printf("File already exists\n");
      return;
    }
    iter -> next = nod;
  }
}

void add_dir(Dir * parent, Dir * nod) {
  if (parent -> head_children_dirs == NULL) {
    parent -> head_children_dirs = nod;
  } else {
    Dir * iter = parent -> head_children_dirs;
    while (iter -> next != NULL) {
      if (strcmp(iter -> name, nod -> name) == 0) {
        destroy_dir(nod);
        printf("Directory already exists\n");
        return;
      }
      iter = iter -> next;
    }
    if (strcmp(iter -> name, nod -> name) == 0) {
      destroy_dir(nod);
      printf("Directory already exists\n");
      return;
    }
    iter -> next = nod;
  }
}

void mkdir(Dir * parent, char * name) {
  Dir * nod = alocdir(parent, name);
  add_dir(parent, nod);
}

void ls(Dir * parent) {
  Dir * iter_dir = parent -> head_children_dirs;
  while (iter_dir != NULL) {
    printf("%s\n", iter_dir -> name);
    iter_dir = iter_dir -> next;
  }
  File * iter_file = parent -> head_children_files;
  while (iter_file != NULL) {
    printf("%s\n", iter_file -> name);
    iter_file = iter_file -> next;
  }
}

void rm(Dir * parent, char * name) {
  File * iter = parent -> head_children_files;
  File * prev;
  if (iter == NULL) {
    printf("Could not find the file\n");
    return;
  }
  while ((strcmp(iter -> name, name) != 0)) {
    prev = iter;
    iter = iter -> next;

    if (iter == NULL)
      break;
  }
  if (iter != NULL) {
    if (iter == parent -> head_children_files)
      parent -> head_children_files = iter -> next;
    else
      prev -> next = iter -> next;
    destroy_file(iter);
  } else
    printf("Could not find the file\n");
  return;
}

void rmdir(Dir * parent, char * name) {
  Dir * iter = parent -> head_children_dirs;
  Dir * prev;
  if (iter == NULL) {
    printf("Could not find the dir\n");
    return;
  }
  while ((strcmp(iter -> name, name) != 0)) {
    prev = iter;
    iter = iter -> next;

    if (iter == NULL)
      break;
  }
  if (iter != NULL) {
    if (iter == parent -> head_children_dirs)
      parent -> head_children_dirs = iter -> next;
    else
      prev -> next = iter -> next;
    destroy_dir(iter);
  } else
    printf("Could not find the dir\n");
  return;
}

void cd(Dir ** target, char * name) {
  Dir * parent = * target;
  if (strcmp(name, "..") == 0) {
    if (parent -> parent != NULL)
      *
      target = (parent -> parent);
    return;
  }

  Dir * iter = parent -> head_children_dirs;
  while (iter != NULL && (strcmp(iter -> name, name) != 0)) {
    iter = iter -> next;
  }
  if (iter == NULL)
    printf("No directories found!\n");
  else {
    * target = iter;
  }
}

void tree(Dir * target, int level) {
  Dir * iter = target -> head_children_dirs;
  while (iter != NULL) {
    for (int i = 0; i < level; i++)
      printf("    ");
    printf("%s\n", iter -> name);
    tree(iter, level + 1);
    iter = iter -> next;
  }
  File * itf = target -> head_children_files;
  while (itf != NULL) {
    for (int i = 0; i < level; i++)
      printf("    ");
    printf("%s\n", itf -> name);
    itf = itf -> next;
  }
}

char * pwd(Dir * target) {
  if (target -> parent == NULL) {
    char * c = malloc(strlen(target -> name) + 5);
    c[0] = '/';
    strcat(c, target -> name);
    return c;
  }
  char * c = pwd(target -> parent);
  c[strlen(c)] = '/';
  c = realloc(c, strlen(c) + strlen(target -> name) + 5);
  return strcat(c, target -> name);
}
void mv_file(File * file, char * name) {
  Dir * parent = file -> parent;
  rm(parent, file -> name);
  touch(parent, name);
  return;
}

void mv_dir(Dir * dir, char * name) {
  Dir * parent = dir -> parent;
  if (parent -> head_children_dirs == dir) {
    parent -> head_children_dirs = dir -> next;
  } else {
    Dir * iter = parent -> head_children_dirs;
    while (iter -> next != dir) {
      iter = iter -> next;
    }
    iter -> next = dir -> next;
  }
  dir -> next = NULL;
  add_dir(parent, dir);
  dir -> name = realloc(dir -> name, strlen(name) + 1);
  strcpy(dir -> name, name);
  return;
}

void mv(Dir * parent, char * oldname, char * newname) {
  Dir * iter_d = parent -> head_children_dirs;
  File * iter_f = parent -> head_children_files;

  while (iter_d != NULL) {
    if (strcmp(iter_d -> name, newname) == 0) {
      printf("File/Director already exists\n");
      return;
    }
    iter_d = iter_d -> next;
  }
  while (iter_f != NULL) {
    if (strcmp(iter_f -> name, newname) == 0) {
      printf("File/Director already exists\n");
      return;
    }
    iter_f = iter_f -> next;
  }

  iter_d = parent -> head_children_dirs;
  iter_f = parent -> head_children_files;

  while (iter_d != NULL) {
    if (strcmp(iter_d -> name, oldname) == 0) {
      mv_dir(iter_d, newname);
      return;
    }
    iter_d = iter_d -> next;
  }
  while (iter_f != NULL) {
    if (strcmp(iter_f -> name, oldname) == 0) {
      mv_file(iter_f, newname);
      return;
    }
    iter_f = iter_f -> next;
  }
  printf("File/Director not found\n");
  return;
}

void init_touch(Dir ** curent) {
  char * c = malloc(100);
  scanf("%s", c);
  touch( * curent, c);
  free(c);
  return;
}
void init_mkdir(Dir ** curent) {
  char * c = malloc(100);
  scanf("%s", c);
  mkdir( * curent, c);
  free(c);
  return;
}
void init_ls(Dir ** curent) {
  ls( * curent);
  return;
}
void init_rm(Dir ** curent) {
  char * c = malloc(300);
  scanf("%s", c);
  rm( * curent, c);
  free(c);
  return;
}
void init_rmdir(Dir ** curent) {
  char * c = malloc(300);
  scanf("%s", c);
  rmdir( * curent, c);
  free(c);
  return;
}
void init_cd(Dir ** curent) {
  char * c = malloc(300);
  scanf("%s", c);
  cd(curent, c);
  free(c);
  return;
}
void init_tree(Dir ** curent) {
  tree( * curent, 0);
  return;
}
void init_pwd(Dir ** curent) {
  char * c = pwd( * curent);
  printf("%s\n", c);
  free(c);
  return;
}
void init_mv(Dir ** curent) {
  char * c1 = malloc(300);
  char * c2 = malloc(300);
  scanf("%s", c1);
  scanf("%s", c2);
  mv( * curent, c1, c2);
  free(c1);
  free(c2);
  return;
}

void select_comand(Dir ** curent, char * s) {
  char comenzi[9][9] = {
    "touch",
    "mkdir",
    "ls",
    "rm",
    "rmdir",
    "cd",
    "tree",
    "pwd",
    "mv"
  };
  void( * init[9])() = {
    init_touch,
    init_mkdir,
    init_ls,
    init_rm,
    init_rmdir,
    init_cd,
    init_tree,
    init_pwd,
    init_mv
  };
  for (int i = 0; i < 9; i++)
    if (strcmp(comenzi[i], s) == 0) {
      ( * init[i])(curent);
      break;
    }
  return;
}

int main() {

  char comand[10];
  scanf("%s", comand);

  Dir * home = alocdir(NULL, "home");
  Dir * curent = home;

  while (strcmp(comand, "stop") != 0) {
    select_comand( & curent, comand);
    scanf("%s", comand);
  }

  destroy_dir(home);
  return 0;
}
