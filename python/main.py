import gtk, gobject, cairo, pango

class Screen:
    def __init__(self):
        self.window = gtk.Window();
        self.window.connect('destroy', gtk.main_quit)
        self.window.set_title("Pengu Q-1")
        self.window.modify_bg(gtk.STATE_NORMAL, gtk.gdk.Color('#ffffff'))
        self.maintable = gtk.Table(10,10)
        self.maintable.set_size_request(300,600)
        self.b1 = FButton("move all")
        self.b2 = FButton("move single")
        self.b3 = FButton("get servodata")
        self.b4 = FButton("dom something else")
        self.maintable.attach(self.b1, 0,1,0,1, gtk.FILL|gtk.EXPAND, gtk.FILL|gtk.EXPAND)
        self.maintable.attach(self.b2, 0,1,1,2, gtk.FILL|gtk.EXPAND, gtk.FILL|gtk.EXPAND)
        self.maintable.attach(self.b3, 0,1,2,3, gtk.FILL|gtk.EXPAND, gtk.FILL|gtk.EXPAND)
        self.maintable.attach(self.b4, 0,1,3,4, gtk.FILL|gtk.EXPAND, gtk.FILL|gtk.EXPAND)
        self.window.add(self.maintable)
        self.window.show_all()
        
    def launch(self):
        gtk.main()

        
class FButton(gtk.Button):
    def __init__(self, text):
        gtk.Button.__init__(self, text)
        self.relief = gtk.RELIEF_NONE
        style = self.get_style().copy()
        style.bg[gtk.STATE_NORMAL] = gtk.gdk.Color('#0075ff')
        self.set_style(style)
        self.child.modify_font(pango.FontDescription("sans bold 12"))
        self.child.modify_fg(gtk.STATE_NORMAL, gtk.gdk.Color('#ffffff'))
        
#-------------------------------------------------
if __name__ == "__main__":
    screen = Screen()
    screen.launch()