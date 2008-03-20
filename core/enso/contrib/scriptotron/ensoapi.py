from enso.messages import displayMessage
from enso import selection

class EnsoApi(object):
    def display_message(self, msg_xml):
        return displayMessage(msg_xml)

    def get_selection(self):
        return selection.get()

    def set_selection(self, seldict):
        return selection.set(seldict)
