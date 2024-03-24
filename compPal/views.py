#import necessary tools + libraries
import requests
from bs4 import BeautifulSoup
from sklearn.neighbors import KNeighborsClassifier, NearestNeighbors
from sklearn.preprocessing import OneHotEncoder
import numpy as np
import pandas as pd
from django.http import JsonResponse
from django.shortcuts import render
from django.views.decorators.http import require_http_methods
from urllib3 import HTTPResponse
from .forms import SurveyForm  # Assuming you have a form class for validation
import re


@require_http_methods(["POST"])
def submit_survey(request):
    laptop_strings = []
    if request.method == 'POST':
        form = SurveyForm(request.POST)
    if form.is_valid():
        import json
        #post_data = json.loads(request.body.decode("utf-8"))
        memorySize = form.cleaned_data['memorySize']
        processor = form.cleaned_data['processor']
        brand = form.cleaned_data['brand']
        screen = form.cleaned_data['screen']
        price = form.cleaned_data['price']
        touch = form.cleaned_data['touch']
        #print(touch)

        # Example: Perform some logic based on selected options
        laptops_info = scrape_best_buy_laptops_v2(laptop_strings)
        top_3_recs = knnFunction(laptops_info, memorySize, processor, brand, screen, touch)
        recommendations = str(laptop_strings[laptops_info.index(top_3_recs[0])]) + "*" + str(laptop_strings[laptops_info.index(top_3_recs[1])]) + "*" + str(laptop_strings[laptops_info.index(top_3_recs[2])])

        
        response_data = {
            'success': True,
            'message': 'Thank you for your submission!',
            'recommendations': recommendations,  # Include additional data based on form input
        }
        return JsonResponse(response_data)
    else:
        return JsonResponse({'success': False, 'errors': form.errors}, status=400)
    

def scrape_best_buy_laptops_v2(laptop_strings):
    headers = {'User-Agent': "Mozilla/5.0 (Windows NT 10.0; Win64; x64) AppleWebKit/537.36 (KHTML, like Gecko) Chrome/42.0.2311.135 Safari/537.36 Edge/12.246"} 
    urls = ['https://www.bestbuy.com/site/searchpage.jsp?id=pcat17071&st=laptops+computers+on+sale', 
            'https://www.bestbuy.com/site/searchpage.jsp?cp=2&id=pcat17071&st=laptops+computers+on+sale', 
            'https://www.bestbuy.com/site/searchpage.jsp?cp=3&id=pcat17071&st=laptops+computers+on+sale', 
            'https://www.bestbuy.com/site/searchpage.jsp?cp=4&id=pcat17071&st=laptops+computers+on+sale']
    
    #laptop_strings = [] #laptop_strings is a list of each product's main listing
    for url in urls: #iterate through all 4 pages 
        response = requests.get(url = url, headers = headers)
        soup = BeautifulSoup(response.content, 'html.parser') # If this line causes an error, run 'pip install html5lib' or install html5lib
        # print(soup.prettify()) 
        for row in soup.findAll('h4', attrs = {'class':'sku-title'}):  # find the names of the laptops
            laptop_strings.append(row.a.text) #add info to dictionary
    
    laptops_info = extract_laptop_info(laptop_strings)
    #lstrings = laptop_strings
    return laptops_info

def extract_laptop_info(laptop_strings):
    laptop_info_list = []
    
    for string in laptop_strings:
        # Initialize dictionary with default values
        laptop_dict = {'brand': "n/a", 'memorySize': "n/a", 'processor': "n/a", 'screen': "n/a", 'touch': "n/a"}
        
        # Extract brand
        brand_match = re.search(r"^\w+", string)
        if brand_match:
            laptop_dict['brand'] = brand_match.group(0)
        
        # Extract memory size
        memory_match = re.search(r"(\d+GB) Memory", string)
        if memory_match:
            laptop_dict['memorySize'] = memory_match.group(1)
        
        # Extract processor
        processor_match = re.search(r"Intel [^\-]+|AMD [^\-]+|M1|M2|M3", string)
        if processor_match:
            laptop_dict['processor'] = processor_match.group(0)
        
        
        # Extract screen size
        screen_match = re.search(r"(\d+(?:\.\d+)?)\"", string)
        if screen_match:
            laptop_dict['screen'] = screen_match.group(1) + '"'
        
        # Extract touchscreen data
        touch_match = re.search(r"2-in-1|Touchscreen|touch", string, re.IGNORECASE)
        if touch_match:
            laptop_dict['touch'] = 'Yes'
        else:
            laptop_dict['touch'] = 'No'
        
        # Append the dictionary to the list
        laptop_info_list.append(laptop_dict)
    
    return laptop_info_list


# Function to convert memory size to numerical data
def memory_size_to_num(memory_size):
    return int(memory_size.replace('GB', ''))

def knnFunction(laptops_info, memorySize, processor, brand, screen,touch):
    memorySize = [laptop['memorySize'] for laptop in laptops_info] + [memorySize]
    brands = [laptop['brand'] for laptop in laptops_info] + [brand]
    processors = [laptop['processor'] for laptop in laptops_info] + [processor]
    screens = [laptop['screen'] for laptop in laptops_info] + [screen]
    touches = [laptop['touch'] for laptop in laptops_info] + [touch]

    encoder = OneHotEncoder()

    # One-hot encode the categorical data
    encoded_memorySize = encoder.fit_transform(np.array(memorySize).reshape(-1, 1)).toarray()
    encoded_brands = encoder.fit_transform(np.array(brands).reshape(-1, 1)).toarray()
    encoded_processors = encoder.fit_transform(np.array(processors).reshape(-1, 1)).toarray()
    encoded_screens = encoder.fit_transform(np.array(screens).reshape(-1, 1)).toarray()
    encoded_touches = encoder.fit_transform(np.array(touches).reshape(-1, 1)).toarray()

    # Convert laptops_info to numerical data
    laptops_numerical = []
    for i, laptop in enumerate(laptops_info):
        try:
            # Attempt to access each encoded array, with a default value in case of IndexError
            memorySize = encoded_memorySize[i] if i < len(encoded_memorySize) else np.zeros((1, encoded_memorySize.shape[1]))
            brands = encoded_brands[i] if i < len(encoded_brands) else np.zeros((1, encoded_brands.shape[1]))
            processors = encoded_processors[i] if i < len(encoded_processors) else np.zeros((1, encoded_processors.shape[1]))
            screens = encoded_screens[i] if i < len(encoded_screens) else np.zeros((1, encoded_screens.shape[1]))
            touches = encoded_touches[i] if i < len(encoded_touches) else np.zeros((1, encoded_touches.shape[1]))
            
            # Concatenate the arrays
            laptops_numerical.append(np.concatenate([
                memorySize,
                brands,
                processors,
                screens,
                touches
            ]))
        except Exception as e:
            # Handle unexpected errors
            print(f"An unexpected error occurred: {e}")

    # Convert user preferences to numerical data
    user_preferences = np.concatenate([
        encoded_memorySize[-1],
        encoded_brands[-1],  # The last entry corresponds to the user's preference
        encoded_processors[-1],
        encoded_screens[-1],
        encoded_touches[-1]
    ])

    # Use NearestNeighbors to find the top 3 laptops that match user preferences
    nbrs = NearestNeighbors(n_neighbors=3, algorithm='auto').fit(laptops_numerical)
    distances, indices = nbrs.kneighbors([user_preferences])

    # Get the top 3 matching laptops
    top_3_laptops = [laptops_info[index] for index in indices[0]]
    print(top_3_laptops)
    return top_3_laptops
    
    



#print(len(laptop_data))
#print(type(laptop_data[0]))
#laptop_data = scrape_best_buy_laptops() 
#print(laptop_data)


@require_http_methods(["GET"])
def demo(request):
    return render(request, "base.html")