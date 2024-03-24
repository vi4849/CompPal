"""
URL configuration for computer_recs project.

The `urlpatterns` list routes URLs to views. For more information please see:
    https://docs.djangoproject.com/en/dev/topics/http/urls/
Examples:
Function views
    1. Add an import:  from my_app import views
    2. Add a URL to urlpatterns:  path('', views.home, name='home')
Class-based views
    1. Add an import:  from other_app.views import Home
    2. Add a URL to urlpatterns:  path('', Home.as_view(), name='home')
Including another URLconf
    1. Import the include() function: from django.urls import include, path
    2. Add a URL to urlpatterns:  path('blog/', include('blog.urls'))
"""
from django.urls import path
from compPal.views import hello_world

urlpatterns = [
    path('hello/', hello_world),
]

from django.urls import path
from .views import submit_survey

urlpatterns = [
    path('submit-survey/', submit_survey, name='submit_survey'),
    # Other URL patterns...
]