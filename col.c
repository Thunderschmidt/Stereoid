


void init_color_table()
{
   int i;
   int buf[MAX_POINTS];
   ColorConversion colortable; 
   colortable.LoadConf("color_tables/editor.conf");
   for(i=0; i<16; i++)
   {
      //links
      col[0][i][0]=colortable.GetValue(SIDE_LEFT, COLOR_RED, i*17);
      col[0][i][1]=colortable.GetValue(SIDE_LEFT, COLOR_GREEN, i*17);
      col[0][i][2]=colortable.GetValue(SIDE_LEFT, COLOR_BLUE, i*17);
      //rechts
      col[1][i][0]=colortable.GetValue(SIDE_RIGHT, COLOR_RED, i*17);
      col[1][i][1]=colortable.GetValue(SIDE_RIGHT, COLOR_GREEN, i*17);
      col[1][i][2]=colortable.GetValue(SIDE_RIGHT, COLOR_BLUE, i*17);
   }
   GLubyte r,g,b;
   colortable.GetBackgnd(&r, &g, &b);
   float r2=r/255.0, g2=r/255.0, b2=r/255.0;
   glClearColor(r2, g2, b2, 1.0);
}

void color3D(bool c, char lumi)
{
}


  

