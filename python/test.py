import crobotlib
import configuration



if __name__ == "__main__":
	crobot = crobotlib.Crobot("./libcrobot64.so.1.0.1")
	config = configuration.Configuration()
	config.load("./crobot.xml")
	crobot.enable_com();
