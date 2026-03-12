/*
PROYECTO NUMERO 2 (EDITOR DE TEXTO)
Alumno: Samuel Hernandez
C.I: 30.030.465

Profesor: Jose Ramirez

Fecha: 25 de enero del 2024

*/

#include <gtk/gtk.h>
#include <windows.h>
#include <hunspell/hunspell.h>

//PROTOTIPOS DE LAS FUNCIONES A UTILIZAR


//////// BOTONES ////////

static void boton_1(GtkButton *button, gpointer user_data); //Boton 1 (Ventana Pricnipal) --> Salir
static void boton_2(GtkButton *button, gpointer user_data); //Boton 2 (Ventana Principal) --> Ayuda
static void boton_3(GtkButton *button, gpointer user_data); //Boton 3 (Ventana Principal) --> Guardar en un .txt
static void boton_4(GtkButton *button, gpointer user_data); //Boton 4 (Ventana Principal) --> Borrar

static GtkWidget *crear_ventana_salir();
static void boton_si_click(GtkWidget *widget, gpointer user_data);
static void boton_cerrar_ventana(GtkWidget *widget, gpointer data);//Sub-Boton (Cuadros de Dialogo) -->  Al presionar ayuda y guardar (ok) y en salir (no). (Misma funcion para los tres botones)


//////// ENCRIPTADO - DESENCRIPTADO ////////

void boton_5(GtkWidget *widget, gpointer user_data);
int proceso_encriptado(char *cadena, int tamano);
void boton_6(GtkWidget *widget, gpointer user_data);


//////// FUNCIONALIDAD ////////

void lim_lineas(GtkTextBuffer *buffer, GtkTextIter *location, gchar *text, gint len, gpointer data);
static void regresar_hoja(GtkWidget *widget, gpointer data); //Permite subir de una hoja a otra
static void scroll_automatico(GtkAdjustment *adjustment, gpointer user_data);//Permite mover el scroll automatico
gboolean bloquear_tab(GtkEventControllerKey *controller, guint keyval, guint keycode, GdkModifierType state, gpointer user_data);//Bloquear el tabulador
void bloquear_pegado(GtkTextView *text_view, gpointer user_data);//Evitar pegado de texto
static void insertar_mayuscula(GtkTextBuffer *buffer, GtkTextIter *ubicacion, gchar *texto, gint longitud, gpointer datos_usuario);


//////// CORRECTOR ORTOGRAFICO ////////

static gboolean subrayar_palabra(GtkEventControllerKey *controller, guint keyval, guint keycode, GdkModifierType state, gpointer user_data);
static void click_cuadro_texto(GtkGestureClick *gesture, int n_press, double x, double y, gpointer user_data);
void correccion_y_sugerencias(GtkTextBuffer *buffer, Hunhandle *handle, const gchar *word);
void reemplazar_palabra(GtkGestureClick *gesture, int n_press, double x, double y, gpointer user_data);
void boton_omitir_correccion(GtkButton *button, gpointer user_data);
void corregir_texto_pegado(GtkTextBuffer *buffer, GtkTextIter *location, gpointer user_data);



//VARIABLES GLOBALES


//ARREGLO DE CARACTERES PARA HACER LA ENCRIPTACION
char caracteres[] = {' ', 'A', 'a', 'B', 'b', 'C', 'c', 'D', 'd', 'E', 'e', 'F', 'f', 'G', 'g', 'H', 'h', 'I', 'i', 'J', 'j', 'K', 'k', 'L', 'l', 'M', 'm', 
'N', 'n', 'O', 'o', 'P', 'p', 'Q', 'q', 'R', 'r', 'S', 's', 'T', 't', 'U', 'u', 'V', 'v', 'W', 'w', 'X', 'x', 'Y', 'y', 'Z', 'z', 
'?', 'É', '?', 'Ó', 'Ú','Ñ', 'á', 'é', 'í', 'ó', 'ú', 'ñ', '¿', '¡', 
'.', ':', ',', ';', '"', '(', ')', '[', ']', '?', '!', '/', '*', '+', '-', '=', '-', '\n','{', '}','%','&','@','$',
 '0', '1', '2', '3', '4', '5', '6', '7', '8', '9', '?', '?', '?', '?', '?', '?', '?', '?', '?', '?', '?', '?'
 };
 
//MATRIZ CLAVE PARA REALIZAR LA ENCRIPTACION
int matriz_clave[3][3] = {
    {1, -2, 2},
    {-1, 1, 3},
    {1, -1, -4}
   
};


//MATRIZ INVERSA DE LA MATRIZ CLAVE PARA LA DESENCRIPTACION
int inversa[3][3] = {
    {-1, -10, -8},
    {-1, -6, -5},
    {0, -1, -1}
};


//Variables para encriptado y desencriptado

int matrices_fila[100000][3]; //Matrices fila sin encriptar que se crean en base al arreglo caracteres
int matrices_fila_ENCRIP[100000][3]; //Matrices fila encriptar que se crean al multiplicar por la matriz clave

//Las variables hoja sirven para insertar el encriptado y desencriptado de forma individual en cada cuadro de texto
int hoja_1[100000][3];
int hoja_2[100000][3];
int hoja_3[100000][3];
int nuevo,totales;
int hoja_4[100000][3];
char mensaje[100000];//Donde se guarda el texto encriptado


GtkWidget *cuadros_texto[4]; //Cuadros de texto
GtkWidget *marcos_cuadros[4]; //Marco para los cuadros de texto
GtkTextBuffer *buffer;        //Buffer para los cuadros de texto

//Estos botones se declaran globales ya que se son los que se desactivan al encriptar
GtkWidget *boton_limpiar;
GtkWidget *boton_encriptar;
GtkWidget *boton_guardar;
GtkWidget *boton_salir;
GtkWidget *boton_si;


GtkWidget *ventana_salir = NULL; //Variable global para manejar el cuadro de dialogo de salir sin guardar
GtkWidget *ventana_ayuda = NULL; //Variable global para manejar el cuadro de dialogo de ayuda
GtkWidget *ventana_guardar = NULL; //Variable global para manejar el cuadro de dialogo de guardar en un .txt


int cambios_sin_guardar = 0; //Variable para saber si se presiona el boton guardar (sino se presiona se crea la ventana de pregunta)
int hojas=0; //Variable para manejar el paso de hojas

GtkTextTag *subrayado = NULL; //Subrayado de las palabras con errores
static gboolean popupOpen = FALSE; //Detectar si la ventana de sugerencias esta abierta o cerrada

int encriptado_si=0; //Variable para saber si se presiona el boton encriptado (sino se presiona se crea la ventana de pregunta)

Hunhandle *handle; //Variable para la inicializacion de hunspell (corrector)


static gboolean detectar_punto = FALSE; //Variable para detectar si hay un punto seguido o a parte

static void activate (GtkApplication* app, gpointer user_data) {

	//Declaracion de wigets y cajas para contenerlos
  	GtkWidget *window_principal;
  	GtkWidget *window_bienvenida;
 
	GtkWidget *scrolled_window;
	GtkWidget *box_principal;
	GtkWidget *box_botones;
  	GtkWidget *box_2;
  	GtkWidget *box_ventana_bienvenida;
 
  	GtkWidget *etiqueta_1;
    GtkWidget *etiqueta_2;
    GtkWidget *etiqueta_3;
    GtkWidget *etiqueta_4;
 
  	//Declaracion de Botones
  	GtkWidget *boton_ayuda;
  	GtkWidget *boton_desencriptar;
 
  	//Proveedor de estilos para el css
  	GtkCssProvider *provider;
 
 
 
  // Inicializaci?n de Hunspell con el diccionario
    handle = Hunspell_create("dic.aff", "dic.dic");

    if (!handle) {
        g_print("No se pudo cargar el diccionario\n");
        return;
    }
 
 

//Creamos el scroll window_principal
scrolled_window = gtk_scrolled_window_new();
    gtk_widget_set_size_request(scrolled_window, 600, 500);
  
 
 
//Creacion de la ventana principal
  window_principal = gtk_application_window_new (app);
  gtk_window_set_title (GTK_WINDOW (window_principal), "EDITOR DE TEXTO");
  gtk_window_set_default_size (GTK_WINDOW (window_principal), 600,500);
  gtk_window_set_deletable(GTK_WINDOW(window_principal), FALSE); //Eliminar la x de la barra
  gtk_window_maximize(GTK_WINDOW(window_principal)); //Permite maximizar la ventana al compilar
 
  //Crearle un borde a la ventana y cambiar su color
	provider = gtk_css_provider_new();
  gtk_css_provider_load_from_data(provider, "* { border: 3px solid black; }" "window { background-color: #D3D3D3; }", -1);
  gtk_style_context_add_provider(gtk_widget_get_style_context(window_principal), GTK_STYLE_PROVIDER(provider), GTK_STYLE_PROVIDER_PRIORITY_USER);  
 
  // Crear una caja vertical principal para organizar los elementos (CAJA PRINCIPAL)
    box_principal = gtk_box_new(GTK_ORIENTATION_VERTICAL,0);
   
  // Crea una nueva caja horizontal para los botones (CAJA BOTONES)
    box_botones = gtk_box_new(GTK_ORIENTATION_HORIZONTAL,5);
   
   
   
   
   
   
   
   
    // Crear tres botones
    boton_salir = gtk_button_new_with_label("Salir");
    boton_ayuda = gtk_button_new_with_label("Ayuda");
    boton_guardar = gtk_button_new_with_label("Guardar");
    boton_limpiar = gtk_button_new_with_label("Borrar texto");
    boton_encriptar = gtk_button_new_with_label("Encriptar");
    boton_desencriptar= gtk_button_new_with_label("Desencriptar");
   
    //// APLICAMOS LA PERSONALIZACION DE LOS BOTONES CON CSS ////
   
    // Crear un proveedor: boton_salir
GtkCssProvider *button1_provider = gtk_css_provider_new();
gtk_css_provider_load_from_string(button1_provider,
    "button {"
    "   color: red; /* Color del texto */"
    "   border: 2px solid #FF5733; /* Borde del bot?n */"
    "   font-size: 15px; /* Tama?o de la fuente */"
    "}");

// Aplicar el proveedor: boton_salir
gtk_style_context_add_provider(gtk_widget_get_style_context(boton_salir), GTK_STYLE_PROVIDER(button1_provider), GTK_STYLE_PROVIDER_PRIORITY_USER);


// Crear proveedor: boton_ayuda
GtkCssProvider *button2_provider = gtk_css_provider_new();
gtk_css_provider_load_from_string(button2_provider,
    "button {"
    "   color: #DAA520; /* Color del texto */"
    "   border: 2px solid #FFD700; /* Borde del bot?n */"
    "   font-size: 15px; /* Tama?o de la fuente */"
    "}");

// Aplicar el proveedor: boton_ayuda
gtk_style_context_add_provider(gtk_widget_get_style_context(boton_ayuda), GTK_STYLE_PROVIDER(button2_provider), GTK_STYLE_PROVIDER_PRIORITY_USER);


// Crear un proveedor: boton_guardar
GtkCssProvider *button3_provider = gtk_css_provider_new();
gtk_css_provider_load_from_string(button3_provider,
    "button {"
    "   color: #006400; /* Color del texto */"
    "   border: 2px solid #4CAF50; /* Borde del bot?n */"
    "   font-size: 15px; /* Tama?o de la fuente */"
    "}");

// Aplicar el proveedor: boton_guardar
gtk_style_context_add_provider(gtk_widget_get_style_context(boton_guardar), GTK_STYLE_PROVIDER(button3_provider), GTK_STYLE_PROVIDER_PRIORITY_USER);


// Crear un proveedor: boton_limpiar (maneja a todos los demas)
GtkCssProvider *button4_provider = gtk_css_provider_new();
gtk_css_provider_load_from_string(button4_provider,
    "button {"
    "   color: blue; /* Color del texto */"
    "   border: 2px solid blue; /* Borde del bot?n */"
    "   border-radius: 4px; /* Bordes redondeados */"
    "   padding: 8px 20px; /* Espaciado interno */"
    "   font-size: 15px; /* Tama?o de la fuente */"
    "}");

// Aplicar el proveedor: boton_encriptar
gtk_style_context_add_provider(gtk_widget_get_style_context(boton_limpiar), GTK_STYLE_PROVIDER(button4_provider), GTK_STYLE_PROVIDER_PRIORITY_USER);


// Crear un proveedor: boton_encriptar
GtkCssProvider *button5_provider = gtk_css_provider_new();
gtk_css_provider_load_from_string(button5_provider,
    "button {"
    "   color: orange; /* Color del texto */"
    "   border: 2px solid orange; /* Borde del bot?n */"
    "   font-size: 15px; /* Tama?o de la fuente */"
    "}");

// Aplicar el proveedor: boton_encriptar
gtk_style_context_add_provider(gtk_widget_get_style_context(boton_encriptar), GTK_STYLE_PROVIDER(button5_provider), GTK_STYLE_PROVIDER_PRIORITY_USER);


// Crear un proveedor: boton_desencriptar
GtkCssProvider *button6_provider = gtk_css_provider_new();
gtk_css_provider_load_from_string(button6_provider,
    "button {"
    "   color: brown; /* Color del texto */"
    "   border: 2px solid brown; /* Borde del bot?n */"
    "   font-size: 15px; /* Tama?o de la fuente */"
    "}");

// Aplicar el proveedor: boton_encriptar
gtk_style_context_add_provider(gtk_widget_get_style_context(boton_desencriptar), GTK_STYLE_PROVIDER(button6_provider), GTK_STYLE_PROVIDER_PRIORITY_USER);

  // Agregar los botones a la caja de botones
    gtk_box_append(GTK_BOX(box_botones), boton_salir);
    gtk_box_append(GTK_BOX(box_botones), boton_ayuda);
    gtk_box_append(GTK_BOX(box_botones), boton_guardar);
    gtk_box_append(GTK_BOX(box_botones), boton_limpiar);
   
    //Separar los botones de encriptacion de los demas
    GtkWidget *message_label4 = gtk_label_new("\t");

    gtk_box_append(GTK_BOX(box_botones), message_label4);
    gtk_box_append(GTK_BOX(box_botones), boton_encriptar);
    gtk_box_append(GTK_BOX(box_botones), boton_desencriptar);
   
    // Agregar la caja de botones a la caja principal
    gtk_box_append(GTK_BOX(box_principal), box_botones);
   
    //LLAMADA A LA SE?AL DE LOS BOTONES
    g_signal_connect(boton_salir, "clicked", G_CALLBACK(boton_1), app);
    g_signal_connect(boton_ayuda, "clicked", G_CALLBACK(boton_2), app);
    g_signal_connect(boton_guardar, "clicked", G_CALLBACK(boton_3), cuadros_texto);
    g_signal_connect(boton_limpiar, "clicked", G_CALLBACK(boton_4), NULL);
    g_signal_connect(boton_encriptar, "clicked", G_CALLBACK(boton_5), NULL);
    g_signal_connect(boton_desencriptar, "clicked", G_CALLBACK(boton_6), NULL);
   
    // Agregamos el scrolled_window directamente a la caja principal
    gtk_box_append(GTK_BOX(box_principal), scrolled_window);
   
//Creamos una caja para contener cuadro de texto y margen
box_2 = gtk_box_new(GTK_ORIENTATION_VERTICAL, 0);
   
    //Centramos el contenido de la caja
gtk_widget_set_halign(box_2, GTK_ALIGN_CENTER); //Centramos horizontalmente
  gtk_widget_set_valign(box_2, GTK_ALIGN_START); // Que este centrado pero en la parte superior de la ventana
 
  //Mensaje tipo etiqueta directo en la ventana

    etiqueta_1 = gtk_label_new("\nSamuel Hernandez C.I: 30.030.465");
    etiqueta_2 = gtk_label_new("PROYECTO: Lab. Lenguaje de Programacion");
   
    gtk_box_append(GTK_BOX(box_2), etiqueta_1);
    gtk_box_append(GTK_BOX(box_2), etiqueta_2);
   
   
   GtkEventController *controller;
  	
 
 
for (int i = 0; i < 4; i++){

    // Creamos los cuadros de texto y establecemos su tama?o
    cuadros_texto[i] = gtk_text_view_new();
    gtk_text_view_set_wrap_mode(GTK_TEXT_VIEW(cuadros_texto[i]), GTK_WRAP_WORD_CHAR);// Para que no sea en una linea infinita
    gtk_text_view_set_justification(GTK_TEXT_VIEW(cuadros_texto[i]), GTK_JUSTIFY_FILL);//Justifica el texto
    gtk_widget_set_size_request(cuadros_texto[i], 550, 20);
    
    ;
   
    //Creamos un marco para contener los cuadros de texto
    marcos_cuadros[i] = gtk_frame_new(NULL);
gtk_widget_set_valign(marcos_cuadros[i], GTK_ALIGN_START); //Mantine el cuadro de texto del mismo tama?o sin importar el tama?o de la ventana
gtk_frame_set_child(GTK_FRAME(marcos_cuadros[i]), cuadros_texto[i]); // Agrega el cuadro de texto al marco

    //Cambiar la fuente a monospace
provider = gtk_css_provider_new();
    gtk_css_provider_load_from_data(provider, "textview { font-family: Monospace; }", -1);
    gtk_style_context_add_provider(gtk_widget_get_style_context(cuadros_texto[i]), GTK_STYLE_PROVIDER(provider), GTK_STYLE_PROVIDER_PRIORITY_APPLICATION);
   
    //Creamos el borde del cuadro de texto
provider = gtk_css_provider_new();
  gtk_css_provider_load_from_data(provider, "* { border: 4px solid black; }", -1);
  gtk_style_context_add_provider(gtk_widget_get_style_context(marcos_cuadros[i]), GTK_STYLE_PROVIDER(provider), GTK_STYLE_PROVIDER_PRIORITY_USER);
 
  // Ocultar los cuadros de texto que no sean necearios
if (i > 0){

         gtk_widget_set_visible(marcos_cuadros[i], FALSE);
         
    }
   
   
    int *index_ptr = g_new(int, 1);
  *index_ptr = i;

   
    //Permite obtener el ajuste vertical para que el scroll se mueva automaticamente
    GtkAdjustment *adjustment = gtk_scrolled_window_get_vadjustment(GTK_SCROLLED_WINDOW(scrolled_window));
   

//Creamos el buffer para el cuadro de texto
buffer = gtk_text_view_get_buffer(GTK_TEXT_VIEW(cuadros_texto[i]));
    gtk_text_buffer_set_text(buffer, "", 0);
   
    //ACA LLAMAMOS A LAS FUNCIONES CORRESPONDIENTES PARA MANEJAR LOS CUADROS DE TEXTO
   
   	g_signal_connect_data(buffer, "insert-text", G_CALLBACK(lim_lineas), index_ptr, (GClosureNotify)g_free, 0);
 
 	     //bloquear el tabulador
	GtkEventController *key_controller = gtk_event_controller_key_new();
	g_signal_connect(key_controller, "key-pressed", G_CALLBACK(bloquear_tab), NULL);
	gtk_widget_add_controller(GTK_WIDGET(cuadros_texto[i]), key_controller);
	
	//Evitar pegado de texto
	g_signal_connect(cuadros_texto[i], "paste-clipboard", G_CALLBACK(bloquear_pegado), NULL);
 
 	//mueve el scroll automatico
    g_signal_connect(adjustment, "changed", G_CALLBACK(scroll_automatico), NULL);
   
 	// Conectar el evento de clic al GtkTextView
    GtkGesture *clickGesture = gtk_gesture_click_new();
    gtk_event_controller_set_propagation_phase(GTK_EVENT_CONTROLLER(clickGesture), GTK_PHASE_TARGET);
    gtk_gesture_single_set_button(GTK_GESTURE_SINGLE(clickGesture), GDK_BUTTON_PRIMARY);
    g_signal_connect(clickGesture, "pressed", G_CALLBACK(click_cuadro_texto), handle);
    gtk_widget_add_controller(cuadros_texto[i], GTK_EVENT_CONTROLLER(clickGesture));
 	
   g_signal_connect(G_OBJECT(buffer), "paste-done", G_CALLBACK(corregir_texto_pegado), handle);

  	g_signal_connect(buffer, "insert-text", G_CALLBACK(insertar_mayuscula), NULL);
   
   	GtkEventController *controller = gtk_event_controller_key_new();
   	gtk_widget_add_controller(cuadros_texto[i], controller);
    g_signal_connect(controller, "key-pressed", G_CALLBACK(subrayar_palabra), cuadros_texto[i]);
    
   
   g_signal_connect(G_OBJECT(buffer), "paste-done", G_CALLBACK(corregir_texto_pegado), handle);

	 /// LLamar a la funcion "on_backspace"
	g_signal_connect(cuadros_texto[i], "backspace", G_CALLBACK(regresar_hoja), NULL);
	
// Establecemos un margen superior de 10 unidades
    gtk_widget_set_margin_top(marcos_cuadros[i], 20);
   
// A?adimos el marco a la caja
    gtk_box_append(GTK_BOX(box_2), marcos_cuadros[i]);
   
   
  }

// Agregamos la caja del cuadro de texto al scrolled_window
gtk_scrolled_window_set_child(GTK_SCROLLED_WINDOW(scrolled_window), box_2);

  // Establecer la expansi?n vertical del scrolled_window a TRUE
gtk_widget_set_vexpand(scrolled_window, TRUE);
 
  //Damos grosor al borde inferior con un salto de linea
etiqueta_3 = gtk_label_new("\n");
    gtk_box_append(GTK_BOX(box_principal), etiqueta_3);
 
//AGREGAMOS LA CAJA PRINCIPAL QUE CONTINE TODO EN LA VENTANA
gtk_window_set_child(GTK_WINDOW(window_principal), box_principal);

//MOSTRAMOS LA VENTANA
gtk_widget_show(window_principal);



//CREACION DE LA VENTANA DE BIENVENIDA

//Creamos una caja para contener la ventana de bienvenida
box_ventana_bienvenida = gtk_box_new(GTK_ORIENTATION_VERTICAL, 0);

//Centramos el mensaje que contendra la ventana
gtk_widget_set_halign(box_ventana_bienvenida , GTK_ALIGN_CENTER); //Centramos horizontalmente
  gtk_widget_set_valign(box_ventana_bienvenida , GTK_ALIGN_CENTER); // Que este centrado pero en la parte superior de la ventana


// Creacion de la ventana de bienvenida
window_bienvenida = gtk_window_new();
gtk_window_set_modal(GTK_WINDOW(window_bienvenida), TRUE);
gtk_window_set_title(GTK_WINDOW(window_bienvenida), "BIENVENIDO");
gtk_window_set_default_size(GTK_WINDOW(window_bienvenida), 200, 200);
gtk_window_set_resizable(GTK_WINDOW(window_bienvenida), FALSE);

//Cambiar el color de la ventana
provider = gtk_css_provider_new();
    gtk_css_provider_load_from_data(provider, "window { background-color: lightyellow; }", -1);
    gtk_style_context_add_provider(gtk_widget_get_style_context(GTK_WIDGET(window_bienvenida)), GTK_STYLE_PROVIDER(provider), GTK_STYLE_PROVIDER_PRIORITY_USER);


etiqueta_4 = gtk_label_new("\t\t\t\t   BIENVENIDO ESTIMADO USUARIO!!! \n\n   Se recomienda presionar el boton de - Ayuda - para conocer lo que puede    \n hacer con este editor de texto  ");
gtk_box_append(GTK_BOX(box_ventana_bienvenida), etiqueta_4);

gtk_window_set_child(GTK_WINDOW(window_bienvenida), box_ventana_bienvenida);

//Mostrar la ventana de bienvenida
gtk_widget_show(window_bienvenida);

}


int main(int argc, char *argv[]) {
   GtkApplication *app;
    int status;

    app = gtk_application_new("org.gtk.example", G_APPLICATION_FLAGS_NONE);

    Hunhandle *handle = Hunspell_create("dic.aff", "dic.dic");
    if (!handle) {
        g_print("No se pudo cargar el diccionario\n");
        return 1;
    }

    g_signal_connect(app, "activate", G_CALLBACK(activate), handle);

    status = g_application_run(G_APPLICATION(app), argc, argv);
    g_object_unref(app);

    Hunspell_destroy(handle);

    return status;
}



//BOTONES

//Boton 1 (Ventana Principal) --> Salir
static void boton_1(GtkButton *button, gpointer user_data) {
   
GtkApplication *app = GTK_APPLICATION(user_data);
GtkTextIter start_iter, end_iter;
GtkTextBuffer *buffer;
gchar *text;

    // Verifica si todos los cuadros de texto est?n vac?os
    gboolean todos_vacios = TRUE;
   
    for (int i = 0; i < 4; i++) {
   
        buffer = gtk_text_view_get_buffer(GTK_TEXT_VIEW(cuadros_texto[i]));
        gtk_text_buffer_get_start_iter(buffer, &start_iter);
        gtk_text_buffer_get_end_iter(buffer, &end_iter);

        text = gtk_text_buffer_get_text(buffer, &start_iter, &end_iter, FALSE);
       
        if (strlen(text) > 0) {
       
            todos_vacios = FALSE;
            g_free(text);
            break;
        }
        g_free(text);
    }

    // Si todos los cuadros de texto est?n vac?os, cierra la aplicaci?n directamente
    if (todos_vacios) {
        g_application_quit(G_APPLICATION(app));
       
    }

//Si no estan vacios, verifica si hay cambios sin guardar, si es asi entonces muestro la advertencia
if (cambios_sin_guardar) {
       
        // Si la ventana ya est? abierta
        if (ventana_salir == NULL) {
       
            ventana_salir = crear_ventana_salir(app);
        }

        // Muestra la ventana
        gtk_widget_show(ventana_salir);
       
    }

else {
        // Si no hay cambios sin guardar, cierra la aplicaci?n directamente
        g_application_quit(G_APPLICATION(app));
    }
}

//Boton 2 (Ventana Principal) --> Ayuda
static void boton_2(GtkButton *button, gpointer user_data) {
   
    GtkWidget *box_principal, *box_etiquetas, *box_boton_ok;
    GtkWidget *etiqueta, *boton_ok;
GtkCssProvider *provider;
GtkCssProvider *etiqueta_provider;
GtkCssProvider *boton_ok_provider;

// Verifica si la ventana ya est? abierta, si lo esta, no se hace nada
    if (ventana_ayuda != NULL) {
        return;
    }


    // Si la ventana no esta abierta, crear una nueva ventana m?s peque?a
    ventana_ayuda = gtk_window_new();
gtk_window_set_modal(GTK_WINDOW(ventana_ayuda), TRUE);// Hacer la ventana emergente modal (no se sobrepone)
    gtk_window_set_default_size(GTK_WINDOW(ventana_ayuda), 630, 200);
    gtk_window_set_decorated (GTK_WINDOW (ventana_ayuda), FALSE);//Eliminamos la barra de titulo de la ventana, para dar semejanza a un cuadro de dialogo
   
   //Crearle un borde a la ventana y cambiar su color
provider = gtk_css_provider_new();
  gtk_css_provider_load_from_data(provider, "* { border: 2px solid black; }" "window { background-color: lightyellow; }", -1);
  gtk_style_context_add_provider(gtk_widget_get_style_context(ventana_ayuda), GTK_STYLE_PROVIDER(provider), GTK_STYLE_PROVIDER_PRIORITY_USER);
 
    // Crear una caja vertical principal para organizar los elementos
    box_principal = gtk_box_new(GTK_ORIENTATION_VERTICAL, 0);
   
    // Crear una caja vertical para las etiquetas
    box_etiquetas = gtk_box_new(GTK_ORIENTATION_VERTICAL, 0);
   

     //Mensaje directo (etiqueta) en la parte superior
    etiqueta = gtk_label_new("\n\t\t\t\t\t   BIENVENIDO ESTIMADO USUARIO!!! \n\n    Lea por favor detenidamente lo que se puede hacer con este editor de texto. \n\n   * Usted podra escribir todo el texto que desee en un maximo de 4 paginas con 22\n    lineas cada una. \n\n   * Cuando termine de escribir si lo desea puede guardar su documento en un archivo \n   de texto.\n\n   * El editor de texto cuenta con corrector ortografico\n\n   *Las palabras con faltas ortograficas se subrayaran en rojo, con click derecho sobre \n     la palabra podra ver las sugerencias disponibles\n\n   *Podra encriptar y desencriptar su texto ademas su texto encriptado se guardara en un\n      archivo de texto individual");

etiqueta_provider = gtk_css_provider_new();
gtk_css_provider_load_from_string(etiqueta_provider,
    "label {"
    "   font-size: 16px; /* Tama?o de la fuente */"
    "   color: solid black; /* Color del texto */"
   "}");

// Aplicar el estilo
gtk_style_context_add_provider(gtk_widget_get_style_context(etiqueta), GTK_STYLE_PROVIDER(etiqueta_provider), GTK_STYLE_PROVIDER_PRIORITY_USER);


//A?adir la etiqueta a la caja de etiquetas
gtk_box_append(GTK_BOX(box_etiquetas), etiqueta);

    // A?adir la caja de etiquetas a la caja principal
    gtk_box_append(GTK_BOX(box_principal), box_etiquetas);

    // Crear una caja horizontal para los botones
    box_boton_ok = gtk_box_new(GTK_ORIENTATION_HORIZONTAL,0);

    // Centrar el contenido de la caja horizontal (boton ok)
    gtk_widget_set_halign(box_boton_ok, GTK_ALIGN_CENTER);

    // Crear el bot?n "Ok"
    boton_ok = gtk_button_new_with_label("Ok");
gtk_widget_set_margin_bottom(box_boton_ok, 20);

// Crear un proveedor de estilo para el bot?n "ok"
boton_ok_provider= gtk_css_provider_new();
gtk_css_provider_load_from_string(boton_ok_provider,
    "button {"
    "   color: black; /* Color del texto */"
    "   border: 1px solid black; /* Borde del bot?n */"
    "   padding: 8px 20px; /* Espaciado interno */"
    "   font-size: 15px; /* Tama?o de la fuente */"
    "}"
    );
   
// Aplicar el proveedor de estilo al bot?n ok
gtk_style_context_add_provider(gtk_widget_get_style_context(boton_ok), GTK_STYLE_PROVIDER(boton_ok_provider), GTK_STYLE_PROVIDER_PRIORITY_USER);

// Llamar a la funcion de la se?al "clicked" del bot?n "Ok"
g_signal_connect(G_OBJECT(boton_ok), "clicked", G_CALLBACK(boton_cerrar_ventana), ventana_ayuda);

    // A?adir el bot?n "Ok" a la caja de botones
    gtk_box_append(GTK_BOX(box_boton_ok), boton_ok);

    // A?adir la caja de botones a la caja principal
    gtk_box_append(GTK_BOX(box_principal), box_boton_ok);

    // Establecer un margen en la parte inferior de la caja de etiquetas
    gtk_widget_set_margin_bottom(box_etiquetas,10);

    // Establecer la caja principal como hijo de la ventana
    gtk_window_set_child(GTK_WINDOW(ventana_ayuda), box_principal);

    // Mostrar la nueva ventana
    gtk_widget_show(ventana_ayuda);
}


//Boton 3 (Ventana Principal) --> Guardar en un .txt
static void boton_3(GtkButton *button, gpointer user_data){

GtkTextIter start_iter, end_iter;
GtkTextBuffer *buffer;
GtkWidget *box_principal, *box_etiquetas, *box_boton_ok;
    GtkWidget *etiqueta, *boton_ok;
    GtkCssProvider *etiqueta_provider;
    GtkCssProvider *provider_boton_ok;
   
GtkCssProvider *provider;
gchar *text;

// Verifica si la ventana ya est? abierta, si lo esta, no se hace nada
    if (ventana_guardar != NULL) {
        return;
    }
   
    else{
   
  GtkWidget *VISOR_TXT = (GtkWidget *)user_data;
  char guardar_archivo[30];
 
  	if(encriptado_si==0)
  	sprintf(guardar_archivo, "Archivo_NORMAL.txt");
 
  	else
  	sprintf(guardar_archivo, "Archivo_ENCRIPTADO.txt");
 
  FILE *archivo_txt = fopen(guardar_archivo, "w");

  for (int x = 0; x < 4; x++)
  {
    buffer = gtk_text_view_get_buffer(GTK_TEXT_VIEW(cuadros_texto[x]));
    gtk_text_buffer_get_start_iter(buffer, &start_iter);
    gtk_text_buffer_get_end_iter(buffer, &end_iter);
   
    text = gtk_text_buffer_get_text(buffer, &start_iter, &end_iter, FALSE);

    // Verifica si el buffer est? vac?o
    if (strlen(text) == 0 && x != 0) // Si est? vac?o y no es el primer cuadro de texto, pasa al siguiente cuadro de texto
    {
      g_free(text);
      continue;  
    }

    // Imprime el borde de asteriscos
    for (int i = 0; i < 60; i++)
    {
      fputc('*', archivo_txt);
     
    }
   
    fputc('\n', archivo_txt);


    // Si el texto est? vac?o, imprime una l?nea vac?a con un asterisco a cada lado
    if (strlen(text) == 0)
    {
      fputc('*', archivo_txt);
      for (int i = 0; i < 55; i++)
      {
        fputc(' ', archivo_txt);
      }
      fputc('*', archivo_txt);
      fputc('\n', archivo_txt);
    }
   
    else
    {
     
      int char_count = 0;
      int text_length = strlen(text);
      int max_line_length = 57;
      int num_lines = (text_length + max_line_length - 1) / max_line_length;

      for (int i = 0; i < num_lines; i++)
      {
        fputc('*', archivo_txt);
        fputc(' ', archivo_txt);

        int line_start_index = i * max_line_length;
        int line_end_index = line_start_index + max_line_length;

        if (line_end_index > text_length)
        {
          line_end_index = text_length;
        }


        for (int j = line_start_index; j < line_end_index; j++)
        {
          if (text[j] == '\n')
          {
         
            for (int k = char_count; k < max_line_length; k++)
            {
              fputc(' ', archivo_txt);
            }
           
           
            fputc('*', archivo_txt);
            fputc('\n', archivo_txt);
            char_count = 0;
            fputc('*', archivo_txt);
            fputc(' ', archivo_txt);
          }
         
          else
          {
            fputc(text[j], archivo_txt);
            char_count++;
          }
        }

        for (int k = char_count; k < max_line_length; k++)
        {
          fputc(' ', archivo_txt);
        }
       
        fputc('*', archivo_txt);
        fputc('\n', archivo_txt);
        char_count = 0;
      }
    }

    g_free(text);  

    // Termina el cuadro para cada GtkTextView //
   
    for (int i = 0; i < 60; i++)
    {
      fputc('*', archivo_txt);
    }
   
    fputc('\n', archivo_txt);

    // Agrega dos saltos de l?nea despu?s de cada cuadro
    fputc('\n', archivo_txt);
    fputc('\n', archivo_txt);
  }

  fclose(archivo_txt);
 
    // Marca que se han guardado los cambios
    cambios_sin_guardar = 0;
   
   if(encriptado_si==0){
   	
   	// Si la ventana no esta abierta, crear una nueva ventana m?s peque?a
    ventana_guardar = gtk_window_new();
gtk_window_set_modal(GTK_WINDOW(ventana_guardar), TRUE);// Hacer la ventana emergente modal (no se sobrepone)
    gtk_window_set_default_size(GTK_WINDOW(ventana_guardar),350, 100);
    gtk_window_set_decorated (GTK_WINDOW (ventana_guardar), FALSE);//Eliminamos la barra de titulo de la ventana, para dar semejanza a un cuadro de dialogo
   
    //Crearle un borde a la ventana y cambiar su color
provider = gtk_css_provider_new();
  gtk_css_provider_load_from_data(provider, "* { border: 2px solid black; } window { background-color: lightgreen; }", -1);
  gtk_style_context_add_provider(gtk_widget_get_style_context(ventana_guardar), GTK_STYLE_PROVIDER(provider), GTK_STYLE_PROVIDER_PRIORITY_USER);
 

    // Crear una caja vertical principal para organizar los elementos
    box_principal = gtk_box_new(GTK_ORIENTATION_VERTICAL, 0);

    // Crear una caja vertical para las etiquetas
    box_etiquetas = gtk_box_new(GTK_ORIENTATION_VERTICAL, 0);
   

    //Mensaje directo (etiqueta) en la parte superior
   etiqueta = gtk_label_new("\nSu documento ha sido guardado con exito. ");
etiqueta_provider = gtk_css_provider_new();
gtk_css_provider_load_from_string(etiqueta_provider,
   "label {"
   "   font-size: 16px; /* Tama?o de la fuente */"
    "   color: solid black; /* Color del texto */"
  "}");

// Aplicar el estilo
gtk_style_context_add_provider(gtk_widget_get_style_context(etiqueta), GTK_STYLE_PROVIDER(etiqueta_provider), GTK_STYLE_PROVIDER_PRIORITY_USER);

//A?adir la etiqueta a la caja de etiquetas
gtk_box_append(GTK_BOX(box_etiquetas), etiqueta);

   // A?adir la caja de etiquetas a la caja principal
    gtk_box_append(GTK_BOX(box_principal), box_etiquetas);

    // Crear una caja horizontal para los botones
    box_boton_ok = gtk_box_new(GTK_ORIENTATION_HORIZONTAL,0);

    // Centrar el contenido de la caja horizontal
    gtk_widget_set_halign(box_boton_ok, GTK_ALIGN_CENTER);

    // Crear el bot?n "Ok"
   boton_ok = gtk_button_new_with_label("Ok");
gtk_widget_set_margin_bottom(box_boton_ok, 20);

// Crear un proveedor de estilo para el bot?n "ok"
provider_boton_ok = gtk_css_provider_new();
gtk_css_provider_load_from_string(provider_boton_ok,
  "button {"
   "   color: black; /* Color del texto */"
    "   border: 1px solid black; /* Borde del bot?n */"
   "   padding: 8px 20px; /* Espaciado interno */"
    "   font-size: 15px; /* Tama?o de la fuente */"
    "}");

// Aplicar el proveedor de estilo al bot?n 1
gtk_style_context_add_provider(gtk_widget_get_style_context(boton_ok), GTK_STYLE_PROVIDER(provider_boton_ok), GTK_STYLE_PROVIDER_PRIORITY_USER);

// Llamar a la funcion de la se?al "clicked" del bot?n "Ok"
g_signal_connect(G_OBJECT(boton_ok), "clicked", G_CALLBACK(boton_cerrar_ventana), ventana_guardar);

    // A?adir el bot?n "Ok" a la caja de botones
    gtk_box_append(GTK_BOX(box_boton_ok), boton_ok);

   // A?adir la caja de botones a la caja principal
   gtk_box_append(GTK_BOX(box_principal), box_boton_ok);

   // Establecer un margen en la parte inferior de la caja de etiquetas
   gtk_widget_set_margin_bottom(box_etiquetas,10);

   // Establecer la caja principal como hijo de la ventana
    gtk_window_set_child(GTK_WINDOW(ventana_guardar), box_principal);

   // Mostrar la nueva ventana
    gtk_widget_show(ventana_guardar);
    Beep(900,100);

   	
   }
    
 
}

}


//Boton 4 (Ventana Principal) --> Borrar
static void boton_4(GtkButton *button, gpointer user_data) {
   
    GtkTextBuffer *buffer;
   
    for (int i = 0; i < 4; i++) {
   
    if (i > 0){
   
         gtk_widget_set_visible(marcos_cuadros[i], FALSE);
         
    }
   
        gtk_text_view_set_editable(GTK_TEXT_VIEW(cuadros_texto[i]),TRUE);  
       
        buffer = gtk_text_view_get_buffer(GTK_TEXT_VIEW(cuadros_texto[i]));
        gtk_text_buffer_set_text(buffer, "", 0);
         
    }
    // Abre el archivo en modo escritura para borrar su contenido
    FILE *guardado_NORMAL = fopen("Archivo_NORMAL.txt", "w");
    FILE *guardado_ENCRIPTADO = fopen("Archivo_ENCRIPTADO.txt", "w");

    // Escribe el primer marco de asteriscos
    for (int i = 0; i < 57; i++) {
        fputc('*', guardado_NORMAL);
        fputc('*', guardado_ENCRIPTADO);
    }
    fputc('\n', guardado_NORMAL);
    fputc('\n', guardado_ENCRIPTADO);

    // Imprime una l?nea vac?a con un asterisco a cada lado
    fputc('*', guardado_NORMAL);
    fputc('*', guardado_ENCRIPTADO);
    
    for (int i = 0; i < 55; i++) {
        fputc(' ', guardado_NORMAL);
        fputc(' ', guardado_ENCRIPTADO);
    }
    fputc('*', guardado_NORMAL);
    fputc('*', guardado_ENCRIPTADO);
    fputc('\n', guardado_NORMAL);
    fputc('\n', guardado_ENCRIPTADO);

    // Imprime el borde inferior de asteriscos
    for (int i = 0; i < 57; i++) {
        fputc('*', guardado_NORMAL);
        fputc('*', guardado_ENCRIPTADO);
    }
    fputc('\n', guardado_NORMAL);
    fputc('\n', guardado_ENCRIPTADO);

    fclose(guardado_NORMAL);
    fclose(guardado_ENCRIPTADO);
}


static GtkWidget *crear_ventana_salir(GtkApplication *app) {

GtkWidget *box_principal, *box_etiquetas, *box_boton_si_no;
    GtkWidget *etiqueta_salir, *boton_no;
    GtkWidget *ventana_salir;
   
GtkCssProvider *provider;


// Si la ventana ya est? abierta, salir de la funcion (evitar que salgan varias ventanas a la vez)
    if (ventana_salir != NULL) {
        gtk_widget_show(ventana_salir);
        return ventana_salir;
    }

    //Si la ventana no esta abierta, crear una nueva ventana
    ventana_salir = gtk_window_new();
gtk_window_set_modal(GTK_WINDOW(ventana_salir), TRUE);// Hacer la ventana emergente modal (se sobrepone)
    gtk_window_set_default_size(GTK_WINDOW(ventana_salir),350, 100);
    gtk_window_set_decorated (GTK_WINDOW (ventana_salir), FALSE);//Eliminamos la barra de titulo de la ventana, para dar semejanza a un cuadro de dialogo
   
    //Crearle un borde a la ventana y cambiar su color
provider = gtk_css_provider_new();
  gtk_css_provider_load_from_data(provider, "* { border: 2px solid black; } window { background-color: lightpink; }", -1);
  gtk_style_context_add_provider(gtk_widget_get_style_context(ventana_salir), GTK_STYLE_PROVIDER(provider), GTK_STYLE_PROVIDER_PRIORITY_USER);
 
    box_principal = gtk_box_new(GTK_ORIENTATION_VERTICAL, 0);// Crear una caja vertical principal para organizar los elementos
    box_etiquetas = gtk_box_new(GTK_ORIENTATION_VERTICAL, 0); // Crear una caja vertical para las etiquetas
   
    //Mensaje directo (etiqueta) en la parte superior
   etiqueta_salir = gtk_label_new("\nSeguro que desea salir sin guardar? \n");
   
    //A?adir la etiqueta a la caja de etiquetas
gtk_box_append(GTK_BOX(box_etiquetas), etiqueta_salir);

   // A?adir la caja de etiquetas a la caja principal
    gtk_box_append(GTK_BOX(box_principal), box_etiquetas);
   
    // Crear una caja horizontal para los botones
    box_boton_si_no = gtk_box_new(GTK_ORIENTATION_HORIZONTAL,10);

    // Centrar el contenido de la caja horizontal
    gtk_widget_set_halign(box_boton_si_no, GTK_ALIGN_CENTER);

    // Crear el bot?n "Si y No"
boton_si = gtk_button_new_with_label("Si");
    boton_no = gtk_button_new_with_label("No");
   
gtk_widget_set_margin_bottom(box_boton_si_no, 20);

//Conectar las se?ales de cda boton
g_signal_connect(boton_si, "clicked", G_CALLBACK(boton_si_click), app);
g_signal_connect(G_OBJECT(boton_no), "clicked", G_CALLBACK(boton_cerrar_ventana), ventana_salir);

    // A?adir los botones "si y no" a la caja de botones
    gtk_box_append(GTK_BOX(box_boton_si_no), boton_si);
    gtk_box_append(GTK_BOX(box_boton_si_no), boton_no);
   

   // A?adir la caja de botones a la caja principal
   gtk_box_append(GTK_BOX(box_principal), box_boton_si_no);

    // Establecer la caja principal como hijo de la ventana
    gtk_window_set_child(GTK_WINDOW(ventana_salir), box_principal);
   
    return ventana_salir;
}

//Sub-Boton (Cuadros de Dialogo) -->  Al presionar ayuda y guardar (ok) y en salir (no). (Misma funcion para los tres botones)
static void boton_cerrar_ventana(GtkWidget *widget, gpointer data) {
   
// Ocultar la ventana en lugar de destruirla
    gtk_widget_hide(GTK_WIDGET(data));
   
    // Restablecer la variable global ventana_ayuda y ventana_guardar
    ventana_ayuda = NULL;
    ventana_guardar = NULL;
    ventana_salir= NULL;

   
}


// Funci?n que se llama cuando se presiona el bot?n "Si" (en la ventana de salir)
static void boton_si_click(GtkWidget *widget, gpointer user_data) {

    GtkApplication *app = GTK_APPLICATION(user_data);
    g_application_quit(G_APPLICATION(app));
   
   
}




//ENCRIPTADO Y DESENCRIPTADO

//Boton 5 (Ventana Principal) --> Encriptado
void boton_5(GtkWidget *widget, gpointer user_data){

  GtkTextBuffer *buffer;
  GtkTextIter start, end;
  char *text;
 

  for (int i = 0; i < 4; i++){

    memset(mensaje, 0, sizeof(mensaje));

    //Leemos el buffer del cuadro de texto
    buffer = gtk_text_view_get_buffer(GTK_TEXT_VIEW(cuadros_texto[i]));
    gtk_text_buffer_get_bounds(buffer, &start, &end);
    text = gtk_text_buffer_get_text(buffer, &start, &end, FALSE);// Obt?n el texto del buffer

    // Si el texto no est? vac?o, lo encriptas
    if (strlen(text) > 0) {
      proceso_encriptado(text, strlen(text));
 
      switch (i)
      {
      case 0:
        memcpy(hoja_1, matrices_fila_ENCRIP, sizeof(matrices_fila_ENCRIP));
        break;
      case 1:
        memcpy(hoja_2, matrices_fila_ENCRIP, sizeof(matrices_fila_ENCRIP));
        break;
      case 2:
        memcpy(hoja_3, matrices_fila_ENCRIP, sizeof(matrices_fila_ENCRIP));
        break;
      case 3:
        memcpy(hoja_4, matrices_fila_ENCRIP, sizeof(matrices_fila_ENCRIP));
        break;
      default:
        // c?digo para el caso por defecto
        break;
      }

      // Establece el texto encriptado en el buffer del text view
      gtk_text_buffer_set_text(buffer, mensaje, -1);
    }

//Si se presiona el boton encriptar y hay texto en el buffer realizamos las acciones, si no hay nada en el buffer no hacemos nada
    if (g_strcmp0(text, "") != 0) {
   
    // Deshabilitar el bot?n "Encriptar", guardar, borrar y salir
    gtk_widget_set_sensitive(boton_encriptar, FALSE);
    gtk_widget_set_sensitive(boton_guardar, FALSE);
    gtk_widget_set_sensitive(boton_salir, FALSE);
    gtk_widget_set_sensitive(boton_limpiar, FALSE);
   
    for (int i = 0; i < 4; i++){

	// Hacer que el cuadro de texto sea de solo lectura
	gtk_text_view_set_editable(GTK_TEXT_VIEW(cuadros_texto[i]), FALSE);
	}

}
    g_free(text);
  }
  
  encriptado_si=1;
  boton_3(NULL,NULL);
  
}

//Logica de encriptacion
int proceso_encriptado(char *cadena, int tamano){

int div,igual,rev=1;                
  char espacio[2] = " ";
  char memoria[100000]; // Buffer temporal para formatear cada númer

  div = (tamano + 2) / 3; // Calcular el número de filas necesarias
  tamano = strlen(cadena);
 
  //Si el residuo es diferente de 0 completamos con un espacio para que la cadena tenga el tama?o correcto
  while (tamano % 3 != 0){
   
strcat(cadena, espacio);
    tamano = strlen(cadena);
  }
 
  //Si el residuo es 0 simplemente tomamos el tama?o
  if (tamano % 3 == 0){
 
    nuevo = tamano;
  }
 
  if (rev == 1){ // si se puede encriptar
 
    //Iniciamos los valores de nuestra matriz en cero
    for (int i = 0; i < 100000; i++){
   
      for (int j = 0; j < 3; j++){
     
        matrices_fila[i][j] = 0;
        matrices_fila_ENCRIP[i][j] = 0;
      }
    }
   
    //En este ciclo comparamos cada caracter del texto con el arreglo de caracteres,
//Y le asignamos el numero correspondiente a cada letra segun el arreglo caracteres
    for (int i = 0; i < tamano; i++) {
   
      for (int j = 0; j < sizeof(caracteres); j++) {
     
        if (cadena[i] == caracteres[j]) {
       
          igual = j;
          matrices_fila[i / 3][i % 3] = igual; // Divide entre 3 para saber en que fila se guarda y por % para la columna
        }
      }
    }
  }

  nuevo = div; // Asegúrate de que 'nuevo' no sea mayor que 3
 
  if (nuevo > 3){
 
    nuevo = 3;
  }

  // Multiplicamos cada matriz fila generada por la matriz clave, de esta manera obtenemos la encriptacion
 
  for (int i = 0; i < div; i++){
 
    for (int j = 0; j < 3; j++){

      matrices_fila_ENCRIP[i][j] = 0; // Reiniciar el valor encriptado2 en cada iteración
      totales++;
      for (int k = 0; k < 3; k++){
     
        matrices_fila_ENCRIP[i][j] += matrices_fila[i][k] * matriz_clave[k][j];
      }
    }
  }


  //Se procede a imprimir el mensaje encriptado
  for (int i = 0; i < div; i++){
 
    for (int j = 0; j < 3; j++){
   
      //encriptado[i][j] = 0;
      sprintf(memoria, "%d ", matrices_fila_ENCRIP[i][j]);
      strcat(mensaje, memoria);

      // Verificamos el tama?o del b?fer mensaje
      if (strlen(mensaje) >= sizeof(mensaje) - 1){
     
        fprintf(stderr, "Error: Desbordamiento de b?fer en mensaje\n");
        return -1; // Indicamos un error
      }
    }
  }

  return 0;
}

//Boton 6 (Ventana Principal) --> Desencriptado
void boton_6(GtkWidget *widget, gpointer user_data){

GtkTextIter start, end;
GtkTextBuffer *buffer;
gchar *text;

int matriz_desencriptada[100000][3];
char desencriptado_individual[4][100000]; //Matriz para almacenar el desencriptado de cada cuadro de texto indivudialmente

    int div = totales / 3;

  for (int i = 0; i < 4; i++){
   
// Obt?n el texto actual del buffer
buffer = gtk_text_view_get_buffer(GTK_TEXT_VIEW(cuadros_texto[i]));
    gtk_text_buffer_get_bounds(buffer, &start, &end);
    text = gtk_text_buffer_get_text(buffer, &start, &end, FALSE);

    if (strlen(text) > 0){
    	
      int (*matriz_encriptada)[3];
      switch (i)
      {
      case 0:
        matriz_encriptada = hoja_1;
        break;
      case 1:
        matriz_encriptada = hoja_2;
        break;
      case 2:
        matriz_encriptada = hoja_3;
        break;
      case 3:
        matriz_encriptada = hoja_4;
        break;
      }

    // Aqu? comienza la funci?n desencriptado
     
      //Hacemas la multiplicacion de las matrices filas codificadas por la matriz inversa de la matriz clave
      //Permitiendo obtener de nuevo las matrices filas sin codificar
for (int i = 0; i < div; i++){

for (int j = 0; j < 3; j++){

matriz_desencriptada[i][j] = 0;
          for (int k = 0; k < 3; k++){
         
          matriz_desencriptada[i][j] += matriz_encriptada[i][k] * inversa[k][j];
         
 }
}
}
   
char mensaje_desencriptado[100000] = ""; //Arreglo para almacenar el mensaje descifrado
int num = sizeof(caracteres) / sizeof(caracteres[0]);


//Se procede a asignar a cada numero, la letra correspondiente del arreglo caracteres
for (int i = 0; i < div; i++){

for (int j = 0; j < 3; j++){

if (matriz_desencriptada[i][j] >= 0 && matriz_desencriptada[i][j] < num){

int valor = matriz_desencriptada[i][j];
            char caracter[200] = {caracteres[valor], '\0'};
            strcat(mensaje_desencriptado, caracter);

}
}
}


//Se eliminan los espacios que se crean al final del texto cuando se desencripta
    int len = strlen(mensaje_desencriptado);
  int count = 0;
 
    for (int i = len - 1; i >= 0; i--){
   
    if (mensaje_desencriptado[i] == ' '){
      count++;
    }
   
    else{
          break;
         
        }
       
    }

    mensaje_desencriptado[len - count] = '\0';

      // Aqu? termina la funci?n desencriptado

      strcpy(desencriptado_individual[i], mensaje_desencriptado);
      gtk_text_buffer_set_text(buffer, desencriptado_individual[i], -1);
    }
   
    for (int i = 0; i < 4; i++){

// Hacer que el cuadro de texto sea de solo lectura
gtk_text_view_set_editable(GTK_TEXT_VIEW(cuadros_texto[i]),TRUE);
}
   
   
	// habilitar el bot?n "Borrar", "Encriptar", guardar y salir
    gtk_widget_set_sensitive(boton_limpiar, TRUE);
    gtk_widget_set_sensitive(boton_guardar, TRUE);
    gtk_widget_set_sensitive(boton_encriptar, TRUE);
    gtk_widget_set_sensitive(boton_salir, TRUE);
    encriptado_si=0;
    
    g_free(text);
  }
  
  //Permitir eliminar el archivo de texto una vez que se desencripta
  if(remove("Archivo_ENCRIPTADO.txt") == 0);
    
}




//FUNCIONALIDADES

//Limitar las lineas y bajar de una hoja a otra
void lim_lineas(GtkTextBuffer *buffer, GtkTextIter *location, gchar *text, gint len, gpointer data){
   
GtkTextIter iter;
GtkTextIter start, end;
    int indice = *(int *)data;
    int cont = 0;
    int ultima_linea;

    gtk_text_buffer_get_start_iter(buffer, &start);
    end = start;

    while (!gtk_text_iter_is_end(&end))
    {
        if (gtk_text_view_starts_display_line(GTK_TEXT_VIEW(cuadros_texto[indice]), &end))
        {
            cont++;
        }

        gtk_text_iter_forward_char(&end);
    }

   
    gtk_text_buffer_get_iter_at_line(buffer, &iter, 21);
    ultima_linea = gtk_text_iter_get_chars_in_line(&iter);

    if (cont == 22 && (ultima_linea >= 57 || strcmp(text, "\n") == 0))
    {      

hojas++;

if (indice < 4)
        {
            indice++;

            gtk_widget_set_visible(cuadros_texto[indice], TRUE);
            gtk_widget_set_visible(marcos_cuadros[indice], TRUE);
            gtk_widget_grab_focus(cuadros_texto[indice]);
        }
           
        if(indice == 4 && cont == 22){
       
for (int i = 0; i < 4; i++){
 
        gtk_text_view_set_editable(GTK_TEXT_VIEW(cuadros_texto[i]), FALSE);  
       
}
 
}
    }
      // Marca que se han realizado cambios
    cambios_sin_guardar = 1;
   
}


//Permite subir de una hoja a otra
static void regresar_hoja(GtkWidget *widget, gpointer data) {

GtkTextIter start_iter, end_iter;
GtkTextBuffer *buffer;
GtkTextBuffer *buffer_anterior;
gchar *text;
 
  buffer = gtk_text_view_get_buffer(GTK_TEXT_VIEW(widget));
  gtk_text_buffer_get_bounds(buffer, &start_iter, &end_iter);
 
  // Obtiene el texto del buffer para verificar si est? vac?o
  text = gtk_text_buffer_get_text(buffer, &start_iter, &end_iter, FALSE);
 
  // Verifica si el cursor est? al principio del buffer
  gboolean cursor_al_principio = gtk_text_iter_get_offset(&start_iter) == 0;
 
  // Verifica si el buffer est? vac?o y si no es la primera p?gina
  if (cursor_al_principio && g_strcmp0(text, "") == 0 && hojas > 0) {
 
    // Oculta la p?gina actual
    gtk_widget_set_visible(marcos_cuadros[hojas], FALSE);
   
    // Disminuye el contador de p?ginas
    hojas--;
   
    // Mueve el foco a la p?gina anterior
    gtk_widget_grab_focus(cuadros_texto[hojas]);
   
    // Coloca el cursor al final del buffer de la p?gina anterior
    buffer_anterior = gtk_text_view_get_buffer(GTK_TEXT_VIEW(cuadros_texto[hojas]));
    gtk_text_buffer_get_end_iter(buffer_anterior, &end_iter);
    gtk_text_buffer_place_cursor(buffer_anterior, &end_iter);
  }

}

// Bloquear el tabulador
gboolean bloquear_tab(GtkEventControllerKey *controller, guint keyval, guint keycode, GdkModifierType state, gpointer user_data) {
    // Verificar si la tecla presionada es Tab
    if (keyval == GDK_KEY_Tab || keyval == GDK_KEY_ISO_Left_Tab) {
        // Bloquear
        return TRUE;
    }

    // Permitir para otras teclas
    return FALSE;
}

//Evitar el pegado de texto
void bloquear_pegado(GtkTextView *text_view, gpointer user_data) {
    g_signal_stop_emission_by_name(text_view, "paste-clipboard");
}

//Permite mover el scroll aotumaticamente
  static void scroll_automatico(GtkAdjustment *adjustment, gpointer user_data){
    // Establecer el valor del ajuste vertical al m?ximo //
   
    gtk_adjustment_set_value(adjustment, gtk_adjustment_get_upper(adjustment) - gtk_adjustment_get_page_size(adjustment));
   
   }

//Mayusculas despues de un punto y seguido o a parte
static void insertar_mayuscula(GtkTextBuffer *buffer, GtkTextIter *ubicacion, gchar *texto, gint longitud, gpointer datos_usuario){
	
    // Si el buffer de texto est? vac?o, el ?ltimo car?cter fue un punto, o el ?ltimo car?cter fue un salto de l?nea, convertir la primera letra a may?scula
    if ((gtk_text_buffer_get_char_count(buffer) == 0 || detectar_punto) && g_ascii_islower(texto[0])){
        texto[0] = g_ascii_toupper(texto[0]);
        detectar_punto = FALSE;
    }
    if (longitud == 1 && (texto[0] == ' ' || texto[0] == '\n')){
        GtkTextIter iter = *ubicacion;
        gtk_text_iter_backward_char(&iter);
        detectar_punto = (gtk_text_iter_get_char(&iter) == '.');
    }
    else if (longitud == 1 && texto[0] == '.'){
        detectar_punto = TRUE;
    }
    else{
        detectar_punto = FALSE;
    }
}




//CORRECTOR

//Permite el subrayado de palabras mal escritas al presionar espacio o enter
static gboolean subrayar_palabra(GtkEventControllerKey *controller, guint keyval, guint keycode, GdkModifierType state, gpointer user_data) {
    GtkTextBuffer *buffer = gtk_text_view_get_buffer(GTK_TEXT_VIEW(user_data));
    gchar *palabra;
    GtkTextIter iter;
    gtk_text_buffer_get_iter_at_mark(buffer, &iter, gtk_text_buffer_get_insert(buffer));
    int linea_cursor = gtk_text_iter_get_line(&iter);

    if (keyval == GDK_KEY_space) {
        GtkTextIter start, end;
        gtk_text_buffer_get_iter_at_mark(buffer, &start, gtk_text_buffer_get_insert(buffer));
        end = start;
        gtk_text_iter_backward_word_start(&start);
        gchar *palabra = gtk_text_buffer_get_text(buffer, &start, &end, FALSE);
        g_strchomp(palabra);

        if (strchr(palabra, ' ') == NULL && Hunspell_spell(handle, palabra) == 0) {
            char **sugerencias;                     //**
            int num_sugerencias = Hunspell_suggest(handle, &sugerencias, palabra);
            GtkTextTag *tag = gtk_text_buffer_create_tag(buffer, NULL, "underline", PANGO_UNDERLINE_ERROR, NULL);

            gtk_text_buffer_apply_tag(buffer, tag, &start, &end);
            Hunspell_free_list(handle, &sugerencias, num_sugerencias);
        }
        g_free(palabra);
    }

    if (keyval == GDK_KEY_Return || keyval == GDK_KEY_KP_Enter) {
        GtkTextIter start, end;
        gtk_text_buffer_get_iter_at_mark(buffer, &start, gtk_text_buffer_get_insert(buffer));
        end = start;
        gtk_text_iter_backward_word_start(&start);
        palabra = gtk_text_buffer_get_text(buffer, &start, &end, FALSE);
        g_strchomp(palabra);

        if (strchr(palabra, '\n') == NULL && Hunspell_spell(handle, palabra) == 0) {
            char **sugerencias;               //**
            int num_sugerencias = Hunspell_suggest(handle, &sugerencias, palabra);
            GtkTextTag *tag = gtk_text_buffer_create_tag(buffer, NULL, "underline", PANGO_UNDERLINE_ERROR, NULL);

            gtk_text_buffer_apply_tag(buffer, tag, &start, &end);
            Hunspell_free_list(handle, &sugerencias, num_sugerencias);
        }
        g_free(palabra);
    }

    return FALSE;
}

//Permite la accion de click derecho sobre el cuadro de texto
static void click_cuadro_texto(GtkGestureClick *gesture, int n_press, double x, double y, gpointer user_data) {
    
    GtkTextView *textView;
    GtkTextBuffer *buffer;
    gchar *word; 
     GtkTextIter iter;
     
     
    textView = GTK_TEXT_VIEW(gtk_event_controller_get_widget(GTK_EVENT_CONTROLLER(gesture)));
    gtk_gesture_single_set_button(GTK_GESTURE_SINGLE(gesture), GDK_BUTTON_SECONDARY);
    
	buffer = gtk_text_view_get_buffer(textView);
    gtk_text_view_get_iter_at_location(textView, &iter, x, y);
    
    gtk_text_buffer_place_cursor(buffer, &iter); // Mover el cursor al lugar del clic
    
    // Verificar si el iterador est? en una palabra
    if (gtk_text_iter_inside_word(&iter)) {
    	
        GtkTextIter start = iter, end = iter;
        gtk_text_iter_backward_word_start(&start);
        gtk_text_iter_forward_word_end(&end);
        word = gtk_text_buffer_get_text(buffer, &start, &end, FALSE);

        // Realizar la verificaci?n ortogr?fica para la palabra seleccionada
        Hunhandle *handle = (Hunhandle *)user_data;
        
        correccion_y_sugerencias(buffer, handle, word);
        g_free(word);
    }
}

// Verifica la ortograf?a de las palabras y muestra sugerencias en una ventana emergente
void correccion_y_sugerencias(GtkTextBuffer *buffer, Hunhandle *handle, const gchar *word) {
    
	gchar *lowercaseWord = g_utf8_strdown(word, -1);
	GtkWidget *window_sugerencias;
	GtkWidget *box_principal;
	gchar *etiqueta;
	gchar *sugerencias;
	
	GtkWidget *boton_omitir;

    if (!Hunspell_spell(handle, lowercaseWord)) {
    	
        char **suggestions; //**
        int numSuggestions = Hunspell_suggest(handle, &suggestions, lowercaseWord);

        if (numSuggestions > 0) {
        	
        	//Crear la ventana de sugerencias
            window_sugerencias = gtk_window_new();
    		gtk_window_set_title(GTK_WINDOW(window_sugerencias), "Sugerencias de Correcci?n");
   			gtk_window_set_default_size(GTK_WINDOW(window_sugerencias), 200, 30);
  			gtk_window_set_modal(GTK_WINDOW(window_sugerencias), TRUE);
    		gtk_window_set_resizable(GTK_WINDOW(window_sugerencias), FALSE);
    		gtk_window_set_deletable(GTK_WINDOW(window_sugerencias), FALSE);
            
        
			//Crear una caja para contener la ventana
			box_principal = gtk_box_new(GTK_ORIENTATION_VERTICAL, 5);
			
			gtk_window_set_child(GTK_WINDOW(window_sugerencias), box_principal);
            
           
            etiqueta = g_strdup_printf("Sugerencias para '%s':", word);
            
            gtk_box_append(GTK_BOX(box_principal), gtk_label_new(etiqueta));
            
            g_free(etiqueta);
            
            for (int i = 0; i < numSuggestions; i++) {
            	
                
                
				if (g_strcmp0(word, g_utf8_strup(word, -1)) == 0) {
    				sugerencias = g_utf8_strup(suggestions[i], -1);
				} 
				
				else {
    				sugerencias = suggestions[i];
				}

                GtkWidget *label = gtk_label_new(sugerencias);
                
                
                GtkGesture *clickGesture = gtk_gesture_click_new();
                gtk_event_controller_set_propagation_phase(GTK_EVENT_CONTROLLER(clickGesture), GTK_PHASE_TARGET);
                gtk_gesture_single_set_button(GTK_GESTURE_SINGLE(clickGesture), GDK_BUTTON_PRIMARY); //Click izquierdo
                g_signal_connect(clickGesture, "pressed", G_CALLBACK(reemplazar_palabra), buffer);
                gtk_widget_add_controller(label, GTK_EVENT_CONTROLLER(clickGesture));
                gtk_box_append(GTK_BOX(box_principal), label);
            }


            boton_omitir = gtk_button_new_with_label("Omitir");
            g_signal_connect(boton_omitir, "clicked", G_CALLBACK(boton_omitir_correccion), buffer);
            gtk_box_append(GTK_BOX(box_principal), boton_omitir);

            gtk_widget_show(window_sugerencias);
            popupOpen = TRUE;

            Hunspell_free_list(handle, &suggestions, numSuggestions);
        }
    }

    g_free(lowercaseWord);
}

// Reemplaza una palabra mal escrita por una sugerencia y maneja el clic en las sugerencias
void reemplazar_palabra(GtkGestureClick *gesture, int n_press, double x, double y, gpointer user_data) {
    
    GtkWidget *etiqueta;
    const gchar *suggestion;
    GtkTextBuffer *buffer;
    GtkWidget *ventana_sugerencias;
    
    
	etiqueta = gtk_event_controller_get_widget(GTK_EVENT_CONTROLLER(gesture));
    suggestion = gtk_label_get_text(GTK_LABEL(etiqueta));

    buffer = GTK_TEXT_BUFFER(user_data);
    GtkTextIter start, end;
    gtk_text_buffer_get_selection_bounds(buffer, &start, &end);

    GtkTextIter wordStart = start;
    
    while (!gtk_text_iter_starts_word(&wordStart)) {
        if (!gtk_text_iter_backward_char(&wordStart)) {
            break;
        }
    }
    
    GtkTextIter wordEnd = end;
    
    while (!gtk_text_iter_ends_word(&wordEnd)) {
        if (!gtk_text_iter_forward_char(&wordEnd)) {
            break;
        }
    }

    gtk_text_buffer_delete(buffer, &wordStart, &wordEnd);
    gtk_text_buffer_insert(buffer, &wordStart, suggestion, -1);

    ventana_sugerencias = gtk_widget_get_ancestor(etiqueta, GTK_TYPE_WINDOW);
    gtk_window_destroy(GTK_WINDOW(ventana_sugerencias));
    popupOpen = FALSE;
}

// Permite omitir una palabra si en ese momento no se desea corregirla 
void boton_omitir_correccion(GtkButton *button, gpointer user_data) {
    // Cerrar la ventana emergente
    GtkWidget *popupWindow = gtk_widget_get_ancestor(GTK_WIDGET(button), GTK_TYPE_WINDOW);
    gtk_window_destroy(GTK_WINDOW(popupWindow));

    // Establecer el estado de popupOpen a FALSE
    popupOpen = FALSE;
}

// Permite verificar la ortograf?a de todo el texto que se pegue en el buffer (cuando hay un copiar_pegar)
void corregir_texto_pegado(GtkTextBuffer *buffer, GtkTextIter *location, gpointer user_data) {
   
    GtkTextIter start, end;
    gtk_text_buffer_get_bounds(buffer, &start, &end);
    
    gchar *word;
    Hunhandle *handle;

    // Mover el iterador al inicio del buffer
    gtk_text_buffer_get_start_iter(buffer, &start);

    while (!gtk_text_iter_is_end(&start)) {
        if (gtk_text_iter_starts_word(&start)) {
            
			GtkTextIter wordStart = start, wordEnd = start;
            gtk_text_iter_forward_word_end(&wordEnd);

           word = gtk_text_buffer_get_text(buffer, &wordStart, &wordEnd, FALSE);

            handle = (Hunhandle *)user_data;
            
            if (!Hunspell_spell(handle, word)) {
            	
                subrayado = gtk_text_buffer_create_tag(buffer, NULL, "underline", PANGO_UNDERLINE_ERROR, NULL);
                gtk_text_buffer_apply_tag(buffer, subrayado, &wordStart, &wordEnd);
                
            }

            g_free(word);
        }

        if (!gtk_text_iter_forward_char(&start)) {
            break;
        }
    }
}


	










