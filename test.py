#! /usr/bin/python

import sys
import subprocess

tests = [
    {
        'input': '69',
        'output': '69'
    },
    {
        'input': '69.',
        'output': '69'
    },
    {
        'input': '69.0',
        'output': '69'
    },
    {
        'input': '12.345',
        'output': '12.345'
    },
    {
        'input': 'FooBar',
        'output': 'FooBar'
    },
    {
        'input': '-deadbeef',
        'output': '-1 * deadbeef'
    },
    {
        'input': '2+3-4.+5.0-6+7',
        'output': '7'
    },
    {
        'input': '2*3/4*5',
        'output': '7.5',
    },
    {
        'input': '2 +3-4*    6   / 5',
        'output': '0.2',
    },
    {
        'input': 'a+b+0+-c+d',
        'output': '-1 * c + a + b + d',
    },
    {
        'input': 'a+(b+0)+c+d',
        'output': 'a + b + c + d',
    },
    {
        'input': 'a*b*0*(c+d)*e',
        'output': '0',
    },
    {
        'input': 'a*(b*0)*(c+d)*e+z',
        'output': 'z',
    },
    {
        'input': 'a*1+b/1',
        'output': 'a + b',
    },
    {
        'input': '(2+3-4)*6/5',
        'output': '1.2',
    },
    {
        'input': '(2+3)*(4-5)',
        'output': '-5',
    },
    {
        'input': '(((2+5)))',
        'output': '7',
    },
    {
        'input': '((2+5) * 6)',
        'output': '42',
    },
    {
        'input': '(x + y) * z',
        'output': '(x + y) * z',
    },
    {
        'input': '(x * y) + z',
        'output': 'x * y + z',
    },
    {
        'input': '2+3-10',
        'output': '-5',
    },
    {
        'input': '2+x-10',
        'output': '-8 + x',
    },
    {
        'input': '2*x/10',
        'output': '0.2 * x',
    },
    {
        'input': '2+x*10',
        'output': '2 + 10 * x',
    },
    {
        'input': '2*x + 10*x - 5*x',
        'output': '7 * x',
    },
    {
        'input': '2*x*y + 3*y*x',
        'output': '5 * x * y',
    },
    {
        'input': '2*x*y + 3*-y*x',
        'output': '-1 * x * y',
    },
    {
        'input': '2*x*y + y*x',
        'output': '3 * x * y',
    },
    {
        'input': '1*x*y - 5*y*x',
        'output': '-4 * x * y',
    },
    {
        'input': '2*x/y + 3*x/y',
        'output': '5 * x / y',
    },
    {
        'input': '2*x*y + 3*y/x',
        'output': '2 * x * y + 3 / x * y',
    },
    {
        'input': '2*(x+1)*(y-2) + 3*(y-2)*(x+1)',
        'output': '5 * (-2 + y) * (1 + x)',
    },
    {
        'input': '2*(x+9+y+-2)*((a+b)*(c+d+e))+3*((a+b)*(c+(d+e)))*(y+x+7)',
        'output': '5 * (7 + x + y) * (a + b) * (c + d + e)',
    },
    {
        'input': '4*(x+19+y-5)*((a+b)*(x+(y+z))) - 10*((x+y+z)*(a+b)*(y+x+7*2))',
        'output': '-6 * (14 + x + y) * (a + b) * (x + y + z)',
    },
    {
        'input': '2*x*y*z + 3*x*y',
        'output': '2 * x * y * z + 3 * x * y',
    },
]

def printSeparator():
    print('\n============\n')

if __name__ == "__main__":
    math_optimizer_bin = 'mathopt'
    if len(sys.argv) > 1:
        math_optimizer_bin = sys.argv[1]
        
    passes = []
    failures = []
    for test in tests:
        cmd = [math_optimizer_bin, test['input']]
        try:
            output = subprocess.check_output(cmd)
        except Exception as e:
            failures.append({
                'test': test,
                'actual': 'Raised error: ' + str(e)
            })
            continue
        output = output.decode('utf-8').strip('\n')
        if (output == test['output']):
            passes.append(test)
        else:
            failures.append({
                'test': test,
                'actual': output
            })

    
    print('PASSES:\n')
    for test in passes:
        print(test['input'] + ' -> ' + test['output'])

    printSeparator()
    print('FAILURES:\n')
    for fail in failures:
        test = fail['test']
        print(test['input'])
        print('\tExpected output: ' + test['output'])
        print('\tActual output: ' + fail['actual'])

    printSeparator()
    print('STATS:')
    print('\tTOTAL: ' + str(len(tests)))
    print('\tPASSES: ' + str(len(passes)))
    print('\tFAILURES: ' + str(len(failures)))
