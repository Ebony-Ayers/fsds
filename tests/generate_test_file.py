import random

RNG_MIN = 0
RNG_MAX = 9
NUM_NUMBERS = 1000000

def main():
	with open("random_numbers.txt", 'w') as outputFile:
		i = 0
		while i < NUM_NUMBERS:
			outputFile.write(str(random.randint(RNG_MIN, RNG_MAX)))
			i += 1

if __name__ == "__main__":
	main()
