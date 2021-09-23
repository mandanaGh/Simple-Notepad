#include <iostream.h>

#include <conio.h>

#include <stdlib.h>

#include <stdio.h>

#include <bios.h>

#include <string.h>

#include <graphics.h>

#include <dos.h>

#include <ctype.h>

#define BORDER 0
#define ESC 27
#define REV_VID 0x20
#define NORM_VID 0x70
//*******************************************************************
int popup(char * menu[], char * keys, int count, int a, int b, int border);
void save_video(int startx, int endx, int starty,
                int endy, unsigned int * buf_ptr);
void draw_border(int startx, int starty, int endx, int endy);
void display_menu(char * menu[], int a, int b, int count);
int get_resp(int a, int b, int count, char * menu[], char * keys);
void restore_video(int startx, int endx, int starty,
                   int endy, unsigned char * buf_ptr);
int is_in(char * s, char c);
void write_video(int a, int b, char * p, int attrib);
void goto_xy(int, int), cls();
void set_border(int color);
void set_mode(int mode);

void type_char(char ch);
void type_list(int a, char ch);
void type_sublist(int a, int b, char ch);
void type(void);
void func1(int a, char ch);
void func2(int a, int b, char ch);
void full_s(int b);
void new_line(struct list * r, int a);
void new_l(int a);
int space(struct list * r);
void enter(void);
void del_char(struct list * r, struct sublist * q1, int a);
int del_line(int a);
void backspace(void);
void delet(void);
void del_l(void);
void save(void);
void open(void);
void null(void);
void graph(void);
void graph1(void);
void get_cur_pos(int page, int * x, int * y);
//*******************************************************************
struct sublist {
    char data1;
    struct sublist * left;
    struct sublist * right;
};

struct list {
    char data;
    struct list * up;
    struct list * down;
    struct sublist * link;
};

char * file[] = {
    " New          ",
    " Open      F3 ",
    " Save      F2 ",
    " Quit         ",
};

char * edit[] = {
    " Cut          ",
    " Copy         ",
    " Paste        ",
    " Clear        ",
};

char * help[] = {
    " About       ",
    " Index       ",
    " Help on help",
};

//*******************************************************************

struct list * l, * p;
struct sublist * l1;
int num[25];
int x, y;
int number = 0;
int number1 = 1;
char string1[30], string2[30];
int v = 0;
//*******************************************************************
void goto_xy(int x, int y) {
    union REGS r;
    r.h.ah = 2;
    r.h.dh = x;
    r.h.dl = y;
    r.h.bh = 0;
    int86(0x10, & r, & r);
}
//*******************************************************************
void set_mode(int mode) {
    union REGS r;
    r.h.al = mode;
    r.h.ah = 0;
    int86(0x10, & r, & r);
}
//*******************************************************************
void set_border(int color) {
    union REGS in , out;
    in .h.ah = 0x0b;
    in .h.bh = 0;
    in .h.bl = color;
    int86(0x10, & in , & out);
}
//*******************************************************************
void get_cur_pos(int page, int * x, int * y) {
    union REGS in , out;
    in .h.ah = 3;
    in .h.bh = page;
    int86(0x10, & in , & out);
    * x = out.h.dh;
    * y = out.h.dl;
}
//*******************************************************************
int main() {
    set_mode(3);
    set_border(10);
    l = (struct list * ) malloc(sizeof(struct list));
    p = (struct list * ) malloc(sizeof(struct list));

    for (int i = 0; i < 25; i++)
        num[i] = 0;

    l -> data = NULL;
    p -> data = NULL;
    l -> up = NULL;
    l -> link = NULL;
    l -> down = p;
    p -> up = l;
    p -> link = NULL;
    p -> down = NULL;
    graph();
    graph1();
    //goto_xy(1,0);

    union inkey {
        char ch[2];
        int i;
    }
    c;
    int m = 1;
    while (m == 1) {
        // while(!bioskey(1));
        c.i = bioskey(0);
        if (c.ch[0]) {
            switch (c.ch[0]) {
            case 27:
                m = 0;
                break;
            case 13:
                enter();
                break;
            case 8:
                backspace();
                break;
            default:
                type_char(c.ch[0]);
            }
        } else
            switch (c.ch[1]) {
            case 83:
                delet();
                break;
            case 32:
                del_l();
                break;
            case 60:
                save();
                break;
            case 61:
                open();
                break;

            case 33:
                get_cur_pos(0, & x, & y);
                textbackground(7);
                int a;
                a = popup(file, "nosq", 4, 1, 2, BORDER);
                if (a == 1) {
                    textbackground(9);
                    textcolor(15);
                    open();
                }

                if (a == 2) {
                    textbackground(9);
                    textcolor(15);
                    save();
                }
                if (a == 3) m = 0;
                textbackground(9);
                textcolor(15);
                goto_xy(x, y);
                break;

            case 18:
                get_cur_pos(0, & x, & y);
                textbackground(7);
                popup(edit, "tcpl", 4, 1, 12, BORDER);
                textbackground(9);
                textcolor(15);
                goto_xy(x, y);
                break;

            case 35:
                get_cur_pos(0, & x, & y);
                textbackground(7);
                popup(help, "aih", 3, 1, 63, BORDER);
                textbackground(9);
                textcolor(15);
                goto_xy(x, y);
                break;

            case 72:
                get_cur_pos(0, & x, & y);
                if ((x - 1 >= 1) && (x <= 23) && (y >= 0) && (y <= 78)) x = x - 1;
                goto_xy(x, y);
                break;

            case 80:
                get_cur_pos(0, & x, & y);
                if ((x >= 1) && (x + 1 <= 23) && (y >= 0) && (y <= 78) && (x + 1 <= number1)) x = x + 1;
                goto_xy(x, y);
                break;

            case 75:
                get_cur_pos(0, & x, & y);
                if ((x >= 1) && (x <= 23) && (y - 1 >= 0) && (y <= 78)) y = y - 1;
                goto_xy(x, y);
                break;

            case 77:
                get_cur_pos(0, & x, & y);
                if ((x >= 1) && (x <= 23) && (y >= 0) && (y + 1 <= 78)) y = y + 1;
                goto_xy(x, y);
                break;
            }
    }

    //goto_xy(10,20);
    //get_cur_pos(0,&x,&y);
    //printf("\n row=%d,col=%d,start=%d,stop=%d",row,column,start,stop);

    //cout<<x<<y;
    //get_resp();

    return 0;
}

//*******************************************************************
void type_char(char ch) {
    get_cur_pos(0, & x, & y);
    int z;
    z = num[x];

    if (y >= z) {
        if (y == 0)
            type_list(x, ch);
        else
            type_sublist(x, y, ch);

        if (y == 78) {
            new_l(x);
            clrscr();
            type();
            goto_xy(x + 1, 0);
        } else
            cout << ch;

    }

    if (y < z) {
        if (y == 0)
            func1(x, ch);
        else
            func2(x, y, ch);

        clrscr();
        type();
        if (y == 78)
            goto_xy(x + 1, 0);
        else
            goto_xy(x, y + 1);
    }
}
//*******************************************************************
void type(void) {
    struct list * r;
    struct sublist * r1;
    r = l;

    graph();
    goto_xy(1, 0);
    while (r) {
        r1 = r -> link;
        cout << r -> data;
        while (r1) {
            cout << r1 -> data1;
            r1 = r1 -> right;
        }
        cout << endl;
        r = r -> down;
    }
}
//*******************************************************************
void full_s(int b) {
    int z1 = 1;
    struct list * r;
    r = l;
    while (r -> down) {
        r = r -> down;
        z1++;
    }

    for (int i = z1 + 1; i < b; i++) {
        struct list * q;
        q = (struct list * ) malloc(sizeof(struct list));
        q -> data = NULL;
        p -> down = q;
        q -> up = p;
        q -> link = NULL;
        q -> down = NULL;
        p = q;
    }
}
//*******************************************************************
void type_list(int a, char ch) {
    if (a == 1) {
        l -> data = ch;
        if (ch != NULL) num[1]++;
    }

    if (a == 2) {
        struct list * r;
        r = l;
        int z1 = 1;
        while (z1 != a) {
            r = r -> down;
            z1++;
        }
        r -> data = ch;
        if (ch != NULL) num[2]++;
    }

    if ((a != 1) && (a != 2)) {
        struct list * r;
        r = l;
        int z1 = 1, z2 = 0;
        while (z1 != a) {
            r = r -> down;
            z1++;
            if (r == NULL) {
                z2 = 1;
                break;
            }
        }
        if (z2 == 1) {
            full_s(a);
            struct list * q;
            q = (struct list * ) malloc(sizeof(struct list));
            q -> data = ch;
            p -> down = q;
            q -> up = p;
            q -> link = NULL;
            q -> down = NULL;
            p = q;
        } else r -> data = ch;
        if (ch != NULL) num[a]++;
    }

}
//*******************************************************************
void type_sublist(int a, int b, char ch) {
    //static int k=0;
    full_s(a);
    struct list * r;
    r = l;
    int z1 = 1;
    while (z1 != a) {
        r = r -> down;
        z1++;
    }
    if (r == NULL) {
        struct list * q;
        q = (struct list * ) malloc(sizeof(struct list));
        q -> data = ' ';
        p -> down = q;
        q -> up = p;
        q -> link = NULL;
        q -> down = NULL;
        p = q;
        r = p;
        num[a]++;
    }

    int s;
    s = num[a];
    struct sublist * p1, * q1;

    if ((s == 0) || (s == 1)) {
        if (s == 0) {
            r -> data = ' ';
            num[a]++;
        }
        for (int i = 1; i <= b; i++) {
            q1 = (struct sublist * ) malloc(sizeof(struct sublist));
            if (i == b)
                q1 -> data1 = ch;
            else
                q1 -> data1 = ' ';

            if (i == 1) {
                q1 -> left = NULL;
                q1 -> right = NULL;
                r -> link = q1;
            } else {
                p1 -> right = q1;
                q1 -> left = p1;
                q1 -> right = NULL;
            }

            p1 = q1;
            num[a]++;
        }
        l1 = p1;
    }

    if ((s != 0) && (s != 1)) {
        p1 = r -> link;
        while (p1 -> right)
            p1 = p1 -> right;

        for (int i = s; i <= b; i++) {
            q1 = (struct sublist * ) malloc(sizeof(struct sublist));

            if (i == b)
                q1 -> data1 = ch;
            else
                q1 -> data1 = ' ';

            p1 -> right = q1;
            q1 -> left = p1;
            q1 -> right = NULL;
            p1 = q1;
            num[a]++;
        }
        l1 = p1;
    }
    //k=a;
}
//*******************************************************************
void func1(int a, char ch) {
    char ch1;
    struct list * r;
    r = l;
    int z1 = 1;
    while (z1 != a) {
        r = r -> down;
        z1++;
    }
    int s;
    s = num[a];
    ch1 = r -> data;
    r -> data = ch;

    struct sublist * q1;
    q1 = (struct sublist * ) malloc(sizeof(struct sublist));
    q1 -> data1 = ch1;
    q1 -> right = r -> link;
    r -> link -> left = q1;
    r -> link = q1;
    q1 -> left = NULL;

    if (s < 79) num[a]++;
    if (s == 79) new_line(r, a);
}
//*******************************************************************
void func2(int a, int b, char ch) {
    char ch1;
    struct list * r;
    r = l;
    int z1 = 1;
    while (z1 != a) {
        r = r -> down;
        z1++;
    }
    int s;
    s = num[a];
    struct sublist * p1, * q1;
    p1 = r -> link;
    for (int i = 1; i < b; i++)
        p1 = p1 -> right;

    ch1 = p1 -> data1;
    p1 -> data1 = ch;
    q1 = (struct sublist * ) malloc(sizeof(struct sublist));
    q1 -> data1 = ch1;
    q1 -> right = p1 -> right;
    p1 -> right -> left = q1;
    p1 -> right = q1;
    q1 -> left = p1;

    if (s < 79) num[a]++;
    if (s == 79) new_line(r, a);

}
//*******************************************************************
void new_line(struct list * r, int a) {
    number1++;
    static int k1 = 1;
    char ch1;
    struct sublist * p1;
    p1 = r -> link;
    while (p1 -> right)
        p1 = p1 -> right;
    ch1 = p1 -> data1;
    p1 -> left -> right = NULL;
    p1 -> left = NULL;
    free(p1);
    if (k1 != a) {
        number = 0;
        k1 = a;
    }
    if (r -> down == NULL) {
        type_list(a + 1, ch1);
        number++;
    } else {
        if (number == 0) {
            struct list * q;
            q = (struct list * ) malloc(sizeof(struct list));
            q -> data = ch1;
            q -> link = NULL;

            r -> down -> up = q;
            q -> down = r -> down;
            r -> down = q;
            q -> up = r;
            for (int i = 22; i > a; i--)
                num[i + 1] = num[i];
            num[a + 1] = 1;
        } else func1(a + 1, ch1);

        number++;
    }
}
//*******************************************************************
void new_l(int a) {
    number1++;
    struct list * r;
    r = l;
    int z1 = 1;
    while (z1 != a) {
        r = r -> down;
        z1++;
    }
    char ch1;
    ch1 = NULL;
    if (r -> down == NULL) type_list(a + 1, ch1);
    else {

        struct list * q, * s;
        q = (struct list * ) malloc(sizeof(struct list));
        q -> data = ch1;
        q -> link = NULL;
        s = r -> down;

        s -> up = q;
        q -> down = s;
        r -> down = q;
        q -> up = r;

        for (int i = 22; i > a; i--)
            num[i + 1] = num[i];
        num[a + 1] = 0;
    }
}
//*******************************************************************
int space(struct list * r) {
    struct sublist * q1;
    int a1 = 0, a2 = 0;
    if (r -> data == ' ') {
        a1++;
        q1 = r -> link;
        a2++;
    }

    while (a2 != 0) {
        if (q1) {
            if (q1 -> data1 == ' ') {
                a1++;
                q1 = q1 -> right;
                a2++;
            } else a2 = 0;
        } else {
            a1 = 0;
            a2 = 0;
        }
    }
    return (a1);
}
//*******************************************************************
void enter(void) {
    int a, b;
    get_cur_pos(0, & x, & y);
    a = x;
    b = y;
    struct list * r;
    r = l;
    int z1 = 1;
    int a1;
    while (z1 != a) {
        r = r -> down;
        z1++;
    }
    int z;
    z = num[a];
    if (z == 0) {
        new_l(a);
        clrscr();
        type();
        goto_xy(a + 1, 0);
    } else {
        a1 = space(r);
        struct sublist * q1;
        if (b >= z) {
            new_l(a);
            clrscr();
            type();
            goto_xy(a + 1, a1);
        } else {
            char ch1;
            if (b == 0) {
                new_l(a);
                ch1 = r -> data;
                r -> data = NULL;
                r -> down -> data = ch1;
                r -> down -> link = r -> link;
                r -> link = NULL;
                num[a + 1] = num[a];
                num[a] = 0;
                clrscr();
                type();
                goto_xy(a + 1, a1);
            } else {
                new_l(a);
                q1 = r -> link;
                z1 = 1;
                while (z1 != b) {
                    q1 = q1 -> right;
                    z1++;
                }
                if (a1 == 0) {
                    ch1 = q1 -> data1;
                    r -> down -> data = ch1;
                    r -> down -> link = q1 -> right;
                    q1 -> right -> left = NULL;
                    q1 -> left -> right = NULL;
                    q1 -> left = NULL;
                    q1 -> right = NULL;
                    if (b == 1) r -> link = NULL;
                    free(q1);
                    num[a + 1] = z - z1;
                    num[a] = z1;
                    clrscr();
                    type();
                    goto_xy(a + 1, a1);
                }
                if (a1 == 1) {
                    r -> down -> data = ' ';
                    r -> down -> link = q1;
                    q1 -> left -> right = NULL;
                    q1 -> left = NULL;
                    if (b == 1) r -> link = NULL;
                    num[a + 1] = z - z1 + 1;
                    num[a] = z1;
                    clrscr();
                    type();
                    goto_xy(a + 1, a1);
                }
                if ((a1 != 0) && (a1 != 1)) {
                    r -> down -> data = ' ';
                    num[a + 1]++;
                    for (int i = 1; i < a1; i++)
                        type_sublist(a + 1, i, ' ');
                    struct sublist * q2;
                    q2 = r -> down -> link;
                    while (q2 -> right)
                        q2 = q2 -> right;

                    q2 -> right = q1;
                    q1 -> left -> right = NULL;
                    q1 -> left = q2;
                    num[a + 1] = num[a + 1] + (z - z1);
                    num[a] = z1;
                    clrscr();
                    type();
                    goto_xy(a + 1, a1);
                }
            }
        }
    }
}
//*******************************************************************
void del_char(struct list * r, struct sublist * q1, int a) {
    if ((q1 -> left != NULL) && (q1 -> right != NULL)) {
        q1 -> left -> right = q1 -> right;
        q1 -> right -> left = q1 -> left;
        q1 -> left = NULL;
        q1 -> right = NULL;
        free(q1);
        num[a]--;
    } else {
        if (q1 -> left == NULL) {
            r -> link = q1 -> right;
            q1 -> right -> left = NULL;
            q1 -> right = NULL;
            free(q1);
            num[a]--;
        } else if (q1 -> right == NULL) {
            q1 -> left -> right = NULL;
            q1 -> left = NULL;
            free(q1);
            num[a]--;
        }
    }
}
//*******************************************************************
int del_line(int a) {
    int a2 = 0;
    char ch1;
    struct list * r;
    r = l;
    int z1 = 1;
    while (z1 != a) {
        r = r -> down;
        z1++;
    }
    int z;
    z = num[a];
    z1 = num[a - 1];
    if (z1 == 0) {
        ch1 = r -> data;
        r -> up -> data = ch1;
        num[a - 1]++;
        r -> up -> link = r -> link;
    }

    if (z1 == 1) {
        ch1 = r -> data;
        type_sublist(a - 1, 1, ch1);
        r -> up -> link -> right = r -> link;
        r -> link -> left = r -> up -> link;
    }

    if ((z1 != 0) && (z1 != 1)) {
        ch1 = r -> data;
        type_sublist(a - 1, z1, ch1);
        struct sublist * q1;
        q1 = r -> up -> link;
        int z2 = 1;
        while (z2 != z1) {
            q1 = q1 -> right;
            z2++;
        }
        q1 -> right = r -> link;
        r -> link -> left = q1;
    }
    a2 = z1;
    r -> link = NULL;
    if (r -> down == NULL) {
        p = r -> up;
        r -> up -> down = NULL;
        r -> up = NULL;
        free(r);
    } else {
        r -> up -> down = r -> down;
        r -> down -> up = r -> up;
        r -> down = NULL;
        r -> up = NULL;
        free(r);
    }
    num[a - 1] = num[a - 1] + (z - 1);
    for (int i = a; i <= 22; i++)
        num[i] = num[i + 1];
    num[23] = 0;

    return (a2);
}
//*******************************************************************
void backspace(void) {
    int a, b, a2 = 0;
    char ch1;
    get_cur_pos(0, & x, & y);
    a = x;
    b = y;
    struct list * r;
    r = l;
    int z1 = 1;
    while (z1 != a) {
        r = r -> down;
        z1++;
    }
    int z;
    z = num[a];
    if (b > 0) {
        if (z == 0) goto_xy(a, 0);
        else {
            int a1;
            a1 = space(r);
            if (b > z) goto_xy(a, b - 1);
            if (b <= z) {
                if (z == 1) {
                    r -> data = NULL;
                    num[a]--;
                }
                if ((z != 1) && (b == 1)) {
                    struct sublist * q1;
                    q1 = r -> link;
                    ch1 = q1 -> data1;
                    r -> data = ch1;
                    r -> link = q1 -> right;
                    q1 -> right -> left = NULL;
                    q1 -> right = NULL;
                    free(q1);
                    num[a]--;
                }

                if ((z != 1) && (b != 1)) {
                    struct sublist * q1;
                    q1 = r -> link;
                    z1 = 2;
                    while (z1 != b) {
                        q1 = q1 -> right;
                        z1++;
                    }
                    if (a1 == 0) del_char(r, q1, a);
                    if ((a1 != 0) && (a1 != b)) del_char(r, q1, a);
                    if (a1 == b) {
                        ch1 = q1 -> right -> data1;
                        struct sublist * r1;
                        r1 = r -> link;
                        r -> data = ch1;
                        r -> link = q1 -> right -> right;
                        q1 -> right -> right -> left = NULL;
                        q1 -> right -> right = NULL;
                        for (int i = 1; i <= a1; i++) {
                            q1 = r1 -> right;
                            r1 -> left = NULL;
                            r1 -> right = NULL;
                            free(r1);
                            r1 = q1;
                        }
                        num[a] = num[a] - a1;
                        a2++;
                    }
                }

                clrscr();
                type();
                if (a2 == 0) goto_xy(a, b - 1);
                else goto_xy(a, 0);
            }
        }
    }
    if ((b == 0) && (a - 1 >= 1)) {
        int w;
        w = del_line(a);
        clrscr();
        type();
        goto_xy(a - 1, w);
    }
}
//*******************************************************************
void delet(void) {
    int a, b;
    get_cur_pos(0, & x, & y);
    a = x;
    b = y;
    struct list * r;
    r = l;
    int z1 = 1;
    while (z1 != a) {
        r = r -> down;
        z1++;
    }
    int z;
    z = num[a];
    if (b < z) {
        if (b == 0) {
            if (z == 1) {
                r -> data = NULL;
                num[a]--;
            } else {
                struct sublist * q1;
                q1 = r -> link;
                r -> data = q1 -> data1;
                q1 -> right -> left = NULL;
                r -> link = q1 -> right;
                q1 -> right = NULL;
                free(q1);
                num[a]--;
            }
        } else {
            struct sublist * q1;
            q1 = r -> link;
            z1 = 1;
            while (z1 != b) {
                q1 = q1 -> right;
                z1++;
            }
            del_char(r, q1, a);
        }
        clrscr();
        type();
        goto_xy(a, b);
    }
}
//*******************************************************************
void del_l(void) {
    int a, b;
    get_cur_pos(0, & x, & y);
    a = x;
    b = y;
    struct list * r;
    r = l;
    int z1 = 1;
    while (z1 != a) {
        r = r -> down;
        z1++;
    }
    int z;
    z = num[a];
    struct sublist * p1, * q1;
    p1 = r -> link;
    if (r -> down == NULL) {
        p = r -> up;
        r -> up -> down = NULL;
        r -> up = NULL;
        r -> link = NULL;
        free(r);
    } else {
        if (r == l) {
            l = r -> down;
            r -> down -> up = NULL;
            r -> down = NULL;
            r -> link = NULL;
            free(r);
        } else {
            r -> up -> down = r -> down;
            r -> down -> up = r -> up;
            r -> up = NULL;
            r -> down = NULL;
            r -> link = NULL;
            free(r);
        }
    }
    for (int i = a; i <= 22; i++)
        num[i] = num[i + 1];
    num[23] = 0;

    for (i = 1; i < z; i++) {
        q1 = p1 -> right;
        p1 -> right = NULL;
        p1 -> left = NULL;
        free(p1);
        p1 = q1;
    }

    clrscr();
    type();
    if (a >= 2) goto_xy(a - 1, 0);
    else goto_xy(a, 0);
}
//*******************************************************************
void save(void) {
    char * s;
    if (v == 0) {
        clrscr();
        graph();
        graph1();
        goto_xy(10, 12);
        cout << "Inter the name and the path's of the file:";
        gets(string2);
        strcpy(s, string2);
        v++;
        clrscr();
        graph();
        graph1();
        type();
    }

    if (v != 0) strcpy(s, string2);

    FILE * in ;
    char ch1;
    in = fopen(s, "w");
    if (! in ) {
        printf("can not open file\n");
        getch();
        exit(1);
    }

    struct list * r;
    struct sublist * r1;
    r = l;
    while (r) {
        r1 = r -> link;
        ch1 = r -> data;
        putc(ch1, in );

        while (r1) {
            ch1 = r1 -> data1;
            putc(ch1, in );
            r1 = r1 -> right;
        }
        putc('\n', in );
        r = r -> down;
    }
    fclose( in );
}
//*******************************************************************
void open(void) {
    int a = 0;
    char * s;
    clrscr();
    graph();
    graph1();
    goto_xy(10, 12);
    cout << "Inter the name and the path's of the file:";
    gets(string1);
    strcpy(s, string1);

    FILE * in ;
    char ch;
    in = fopen(s, "r");
    if (! in ) {
        goto_xy(11, 12);
        printf("can not open file\n");
        getch();
        clrscr();
        graph();
        graph1();
        type();
        a++;
    }

    if (a == 0) {
        null();
        clrscr();
        graph();
        goto_xy(1, 0);

        ch = getc( in );
        int a = 1, b = 0;
        while (ch != EOF) {
            if (ch == '\n') {
                a++;
                b = 0;
            } else {
                if (b == 0) {
                    type_list(a, ch);
                    b++;
                } else {
                    type_sublist(a, b, ch);
                    b++;
                }
            }
            ch = getc( in );
        }
        fclose( in );
        //textbackground(9);
        //textcolor(15);
        type();
        goto_xy(a, 0);
        number1 = a;
    }
}
//*******************************************************************
void null(void) {
    number1 = 1;
    number = 0;
    struct list * r, * q;
    struct sublist * r1, * q1;
    r = l;

    while (r) {
        q = r -> down;
        r1 = r -> link;
        r -> up = NULL;
        r -> link = NULL;
        r -> down = NULL;
        free(r);
        while (r1) {
            q1 = r1 -> right;
            r1 -> right = NULL;
            r1 -> left = NULL;
            free(r1);
            r1 = q1;
        }
        r = q;
    }
    l = (struct list * ) malloc(sizeof(struct list));
    p = (struct list * ) malloc(sizeof(struct list));

    for (int i = 0; i < 25; i++)
        num[i] = 0;

    l -> data = NULL;
    p -> data = NULL;
    l -> up = NULL;
    l -> link = NULL;
    l -> down = p;
    p -> up = l;
    p -> link = NULL;
    p -> down = NULL;
}
//*******************************************************************
void graph(void) {
    int i, j;
    goto_xy(0, 0);
    textbackground(7);
    for (i = 0; i <= 79; i++)
        cprintf(" ");
    goto_xy(24, 0);
    for (i = 0; i <= 79; i++)
        cprintf(" ");

    goto_xy(0, 6);
    textcolor(4);
    cprintf(" F");
    textcolor(0);
    cprintf("ile ");

    goto_xy(0, 16);
    textcolor(4);
    cprintf(" E");
    textcolor(0);
    cprintf("dit ");

    goto_xy(0, 66);
    textcolor(4);
    cprintf(" H");
    textcolor(0);
    cprintf("elp ");

    goto_xy(24, 2);
    textcolor(4);
    cprintf("F1");
    goto_xy(24, 14);
    cprintf("F2");
    goto_xy(24, 26);
    cprintf("F3");
    goto_xy(24, 70);
    cprintf("Esc");

    goto_xy(24, 5);
    textcolor(0);
    cprintf("Help");
    goto_xy(24, 17);
    cprintf("Save");
    goto_xy(24, 29);
    cprintf("Open");
    goto_xy(24, 74);
    cprintf("Quit");
    textbackground(9);
    textcolor(15);

}
//*******************************************************************
void graph1(void) {
    int i, j;
    goto_xy(1, 0);
    textbackground(9);
    textcolor(15);
    for (i = 1; i <= 23; i++)
        for (j = 0; j <= 79; j++)
            cprintf(" ");

    goto_xy(1, 0);
    textcolor(15);
}
//*******************************************************************
int popup(char * menu[], char * keys, int count, int a, int b, int border) {
    register int i, len;
    int endx, endy, choice;
    unsigned int * p;

    if ((a > 24) || (a < 0) || (b > 79) || (b < 0)) {
        printf("\n range error");
        return -2;
    }

    len = 0;
    for (i = 0; i < count; i++)
        if (strlen(menu[i]) > len)
            len = strlen(menu[i]);

    endy = len + 2 + b;
    endx = count + 1 + a;
    if ((endx + 1 > 24) || (endy + 1 > 79)) {
        printf("\n menu won't fit");
        return -2;
    }
    p = (unsigned int * ) malloc((endx - a + 1) * (endy - b + 1));
    if (!p)
        exit(1);

    save_video(a, endx + 1, b, endy + 1, p);
    if (border)
        draw_border(a, b, endx, endy);
    display_menu(menu, a + 1, b + 1, count);
    choice = get_resp(a + 1, b, count, menu, keys);
    restore_video(a, endx + 1, b, endy + 1, (char * ) p);
    free(p);
    return choice;
}
//*******************************************************************
void display_menu(char * menu[], int a, int b, int count) {
    register int i;
    for (i = 0; i < count; i++, a++) {
        goto_xy(a, b);
        textcolor(0);
        cprintf(menu[i]);
    }
}
//*******************************************************************
void draw_border(int startx, int starty, int endx, int endy) {
    register int i;
    for (i = startx + 1; i < endx; i++) {
        goto_xy(i, starty);
        putchar(179);
        goto_xy(i, endy);
        putchar(179);
    }
    for (i = starty + 1; i < endy; i++) {
        goto_xy(startx, i);
        putchar(196);
        goto_xy(endx, i);
        putchar(196);
    }
    goto_xy(startx, starty);
    putchar(218);
    goto_xy(startx, endy);
    putchar(191);
    goto_xy(endx, starty);
    putchar(192);
    goto_xy(endx, endy);
    putchar(217);
}
//*******************************************************************
int get_resp(int a, int b, int count, char * menu[], char * keys) {
    union inkey {
        char ch[2];
        int i;
    }
    c;

    int arrow_choice = 0, key_choice;
    b++;
    goto_xy(a, b);
    write_video(a, b, menu[0], REV_VID);
    for (;;) {
        while (!bioskey(1));
        c.i = bioskey(0);
        goto_xy(a + arrow_choice, b);
        write_video(a + arrow_choice, b, menu[arrow_choice], NORM_VID);
        if (c.ch[0]) {
            /* normal key */
            key_choice = is_in(keys, tolower(c.ch[0]));
            if (key_choice)
                return key_choice - 1;
            switch (c.ch[0]) {
            case '\r':
                return arrow_choice;
            case ' ':
                arrow_choice++;
                break;
            case ESC:
                return -1; /* cancel */
            } //end of switch
        } //end of if
        else {
            switch (c.ch[1]) {
            case 72:
                arrow_choice--;
                break;
            case 80:
                arrow_choice++;
                break;
            } //end of switch
        } // end of else
        if (arrow_choice == count)
            arrow_choice = 0;
        if (arrow_choice < 0)
            arrow_choice = count - 1;
        goto_xy(a + arrow_choice, b);
        write_video(a + arrow_choice, b, menu[arrow_choice], REV_VID);
    } //end of for(;;)
}
//*******************************************************************
void write_video(int a, int b, char * p, int attrib) {
    register int i, j;
    union REGS r;
    for (i = b; * p; i++) {
        goto_xy(a, i);
        r.h.ah = 9;
        r.h.bh = 0;
        r.x.cx = 1;
        r.h.al = * p++;
        r.h.bl = attrib;
        int86(0x10, & r, & r);
    }
}
//*******************************************************************
void save_video(int startx, int endx, int starty,
                int endy, unsigned int * buf_ptr) {
    register int i, j;
    union REGS r;
    for (i = starty; i < endy; i++)
        for (j = startx; j < endx; j++) {
            goto_xy(j, i);
            r.h.ah = 8;
            r.h.bh = 0;
            * buf_ptr++ = int86(0x10, & r, & r);
            putchar(' ');
        }
}
//*******************************************************************
void restore_video(int startx, int endx, int starty,
                   int endy, unsigned char * buf_ptr) {
    register int i, j;
    union REGS r;
    for (i = starty; i < endy; i++)
        for (j = startx; j < endx; j++) {
            goto_xy(j, i);
            r.h.ah = 9;
            r.h.bh = 0;
            r.x.cx = 1;
            r.h.al = * buf_ptr++;
            r.h.bl = * buf_ptr++;
            int86(0x10, & r, & r);
        }
}
//*******************************************************************
void cls() {
    union REGS r;
    r.h.ah = 6;
    r.h.al = 0;
    r.h.ch = 0;
    r.h.cl = 0;
    r.h.dh = 24;
    r.h.dl = 79;
    r.h.bh = 7;
    int86(0x10, & r, & r);
}
//*******************************************************************
int is_in(char * s, char c) {
    register int i;
    for (i = 0; * s; i++)
        if ( * s++ == c)
            return i + 1;
    return 0;
}
//*******************************************************************