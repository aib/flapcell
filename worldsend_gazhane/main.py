import queue
import re
import threading

import pythonosc.dispatcher
import pythonosc.osc_server
import serial
import serial.tools.list_ports

class MessageSender:
	def __init__(self):
		self.q = queue.Queue()
		self.serials = []

		threading.Thread(name="MessageSender", target=self._start, daemon=True).start()

	def send_message(self, message):
		sat_name = sat_name_str(message)
		self.q.put(sat_name)

	def _start(self):
		while True:
			self._get_serials()

			msg = self.q.get()

			for i, ser in enumerate(self.serials):
				try:
					ser.write(msg.encode('ascii') + b'\r')
				except Exception as e:
					print("Cannot write to port, removing:", e)
					try:
						ser.close()
					except: pass
					del self.serials[i]

	def _get_serials(self):
		for port, desc, hwid in serial.tools.list_ports.comports():
			if any([port in ser.port for ser in self.serials]):
				continue

			try:
				ser = serial.Serial(port, 115200, timeout=1)
			except Exception as e:
				print("Cannot open port, continuing:", e)
				continue

			intro = ser.readline()

			if intro.startswith(b'Serial Driver'):
				self.serials.append(ser)
				continue

			ser.close()

def sat_name_str(name):
	ALPHABET = ' ABCDEFGHIJKLMNOPQRSTUVWXYZ23456789@*-.,#gyrmcsoab'
	BASE = 'ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/'
	SUBST = {
		'0': 'O',
		'1': 'I',
		'\'': ',',
	}

	def remove_parentheses(name):
		m = re.fullmatch(r'([^(]+) \(([^)]+)\)', name)
		if m is not None:
			return (m.group(1), m.group(2))

		m = re.fullmatch(r'([^(]+) \[([^)]+)\]', name)
		if m is not None:
			return (m.group(1), m.group(2))

		return (name, None)

	def normalize_name(name):
		name = remove_parentheses(name)[0]
		name = name.replace(" & ", " ")
		if len(name) > 14:
			name = name[:14]
		return name

	def string_to_flap(s, subst={}, fallback=None):
		flap_str = ''
		for c in s.upper():
			if c in ALPHABET:
				flap_str += c
			elif c in subst:
				flap_str += subst[c]
			elif c in SUBST:
				flap_str += SUBST[c]
			elif fallback is not None:
				flap_str += fallback
			else:
				raise Exception(f"Cannot convert '{c}' in \"{s}\"")

		return flap_str

	def flap_to_index(flaps):
		return ''.join([BASE[ALPHABET.index(c)] for c in flaps])

	first = remove_parentheses(name)[0]
	norm = normalize_name(first)
	flap = string_to_flap(norm, subst={ '+': '-', '/': '', '&': '', '_': '-' })
	indices = flap_to_index(flap)

	return indices

message_sender = None

def osc_message(message, *args):
	if message == '/satellite-in':
		name = args[2]
		message_sender.send_message(name)

def main():
	global message_sender

	message_sender = MessageSender()

	disp = pythonosc.dispatcher.Dispatcher()
	disp.set_default_handler(osc_message)
	osc_server = pythonosc.osc_server.BlockingOSCUDPServer(('0.0.0.0', 7401), disp)
	osc_server.serve_forever()

if __name__ == '__main__':
	main()
