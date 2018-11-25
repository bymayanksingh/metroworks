#include <gtk/gtk.h>
#include<bits/stdc++.h>
using namespace std;
static void button_clicked(GtkWidget* widget,gpointer data)//data is what we passed rather than NULL used in g_connect call
{
  cout<<"Button is pressed\n";
}

int main(int argc,char* argv[]){
  gtk_init(&argc,&argv);
  GtkWidget *window,*label,*button;
  window=gtk_window_new(GTK_WINDOW_TOPLEVEL);//We created a window which is a top level window
 
 
  //label=gtk_label_new("Metro Works");
  
 
  button=gtk_button_new_with_label("Find the Shortest Route");//It will create a empty button
  g_signal_connect(window,"delete-event",G_CALLBACK(gtk_main_quit),NULL);//connect to a function and call another function
  g_signal_connect(button,"clicked",G_CALLBACK(button_clicked),NULL);
 
 
 // gtk_container_add(GTK_CONTAINER(window),label);//require 2 param widget,container
 
 
  gtk_container_add(GTK_CONTAINER(window),button);
  gtk_widget_set_size_request(window,1000,1000);//Requesting for change the size of window
 
  gtk_widget_show_all(window); 


  gtk_main();
  return 0;
}