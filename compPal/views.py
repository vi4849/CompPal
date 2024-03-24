#import necessary tools + libraries
import requests
from bs4 import BeautifulSoup
from sklearn.neighbors import KNeighborsClassifier
import pandas as pd
from django.http import JsonResponse
from django.shortcuts import render
from django.views.decorators.http import require_http_methods
from urllib3 import HTTPResponse
from .forms import SurveyForm  # Assuming you have a form class for validation

@require_http_methods(["POST"])
def submit_survey(request):
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
        data_dict = scrape_best_buy_laptops()
        knnFunction(data_dict)
        #call makerecommendation(memorySize, processor, brand, screen, price,touch)
        recommendation = "The intel one"

        response_data = {
            'success': True,
            'message': 'Thank you for your submission!',
            'recommendation': recommendation,  # Include additional data based on form input
        }
        return JsonResponse(response_data)
    else:
        return JsonResponse({'success': False, 'errors': form.errors}, status=400)
    

#scrapes the first four pages of the best buy website 
#scrapes the first four pages of the best buy website 
def scrape_best_buy_laptops():
    headers = {'User-Agent': "Mozilla/5.0 (Windows NT 10.0; Win64; x64) AppleWebKit/537.36 (KHTML, like Gecko) Chrome/42.0.2311.135 Safari/537.36 Edge/12.246"} 
    urls = ['https://www.bestbuy.com/site/searchpage.jsp?id=pcat17071&st=laptops+computers+on+sale', 
            'https://www.bestbuy.com/site/searchpage.jsp?cp=2&id=pcat17071&st=laptops+computers+on+sale', 
            'https://www.bestbuy.com/site/searchpage.jsp?cp=3&id=pcat17071&st=laptops+computers+on+sale', 
            'https://www.bestbuy.com/site/searchpage.jsp?cp=4&id=pcat17071&st=laptops+computers+on+sale']
    
    laptops = [] #laptop_data is a dictionary with key-value pairs (key = name)
    for url in urls: #iterate through all 4 pages 
        response = requests.get(url = url, headers = headers)
        soup = BeautifulSoup(response.content, 'html.parser') # If this line causes an error, run 'pip install html5lib' or install html5lib
        # print(soup.prettify()) 
        for row in soup.findAll('h4', attrs = {'class':'sku-title'}):  # find the names of the laptops
            laptops.append(row.a.text) #add the key-value pairs (ex. name-"lenovo thinkpad...") to the dictionary

    brand_list=[]
    mem_list = []
    processor_list = []
    screen_list = []
    touch_list = []

    laptop_dictionary={"brand":brand_list, "memorySize":mem_list, "processor":processor_list, 
                       "screen":screen_list, "touch": touch_list}

    for i in laptops:
        split = i.split('- ')
        if(len(split)<5):
            split = i.split(' -')

        #print(len(split))
        split_2 = split[1].split('"')
        #for j in split:
         #   print(j)
        #correct up to this point 

        brand_list.append(split[0])
        if  len(split) < 5:
            mem_list.append("N/A")
        else:
            mem_list.append(split[3])
        processor_list.append(split[2])
        screen_list.append(split_2[0])
        if len(split_2)>1:
            if "touch" in split_2[1].lower():
                touch_list.append("true")
            else:
               touch_list.append("false")     
        else:
            touch_list.append("false")
        print(laptop_dictionary)

    
    """
    for key, value_list in laptop_dictionary.items():
        print(f"Elements for {key}:")
        for element in value_list:
                print(element)
        print("--------")
    """
    return laptop_dictionary

def makeRecommendation(brand,memorySize, processor, screen,touch):
    return "the intel one"

def knnFunction(data_dict):
    df = pd.DataFrame(data_dict)
    df_encoded = pd.get_dummies(df, columns=['brand', 'memorySize','processor','screen','touch'])
    



#print(len(laptop_data))
#print(type(laptop_data[0]))
#laptop_data = scrape_best_buy_laptops() 
#print(laptop_data)


@require_http_methods(["GET"])
def demo(request):
    return render(request, "base.html")