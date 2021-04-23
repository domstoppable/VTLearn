// Fill out your copyright notice in the Description page of Project Settings.

#include "SeafileClient.h"

#include "Runtime/Online/HTTP/Public/Http.h"
#include "Json.h"
#include "Serialization/JsonReader.h"

#include "Misc/Paths.h"
#include "Misc/FileHelper.h"

//#include "Base64.h"

TSharedRef<IHttpRequest> USeafileClient::CreateRequest(FString Route, FString Method)
{
	TSharedRef<IHttpRequest> Request = FHttpModule::Get().CreateRequest();

	if (!Route.Contains("://"))
	{
		Route = this->BaseURL + Route;
	}
	Request->SetURL(Route);
	Request->SetVerb(Method);

	UE_LOG(LogTemp, Log, TEXT("%s request to %s"), *Method, *Route);

	Request->SetHeader(TEXT("Content-Type"), TEXT("application/x-www-form-urlencoded"));
	Request->SetHeader(TEXT("Authorization"), TEXT("Token " + this->Token));

	return Request;
}

void USeafileClient::HandleAuthResponse(FHttpRequestPtr Request, FHttpResponsePtr Response, bool Success)
{
	FHTTPActionComplete* Callback = this->AuthenticationCallbacks.Find(Request);

	if(Success && Response.IsValid())
	{
		if (!Success || !EHttpResponseCodes::IsOk(Response->GetResponseCode()))
		{
			UE_LOG(LogTemp, Error, TEXT("Seafile authentication failed! %d %d"), Success, Response->GetResponseCode());
		}else{
			TSharedPtr<FJsonObject> ParsedResponse;
			TSharedRef<TJsonReader<TCHAR>> JsonReader = TJsonReaderFactory<TCHAR>::Create(Response->GetContentAsString());
			if (FJsonSerializer::Deserialize(JsonReader, ParsedResponse))
			{
				this->Token = ParsedResponse->GetStringField("token");
			}
		}
	}

	if(Callback)
	{
		this->AuthenticationCallbacks.Remove(Request);
		(*Callback).ExecuteIfBound(this->Token != "");
	}
}

void USeafileClient::Authenticate(FString Server, FString Username, FString Password, const FHTTPActionComplete& OnAuthComplete)
{
	this->BaseURL = Server + "/api2/";
	this->Token = "";

	TSharedRef<IHttpRequest> Request = this->CreateRequest("auth-token/", "POST");
	this->AuthenticationCallbacks.Add(Request, OnAuthComplete);

	Request->SetContentAsString("username=" + Username + "&password=" + Password);
	Request->OnProcessRequestComplete().BindUObject(this, &USeafileClient::HandleAuthResponse);

	Request->ProcessRequest();
}

void USeafileClient::HandleGenericHTTPResponse(FHttpRequestPtr Request, FHttpResponsePtr Response, bool Success)
{
	FUploadRequestInfo* UploadRequestInfo = *this->UploadLinkRequests.Find(Request);
	bool Failed = false;

	if(Success && Response.IsValid())
	{
		UE_LOG(LogTemp, Log, TEXT("Received HTTP response %d %d"), Success, Response->GetResponseCode());

		if (UploadRequestInfo != NULL)
		{
			if (UploadRequestInfo->State.Equals("gettingLink"))
			{
				if (Response->GetResponseCode() == 404)
				{
					UE_LOG(LogTemp, Log, TEXT("Need to create remote folder"));
					this->Mkdir(UploadRequestInfo);

				} else if(EHttpResponseCodes::IsOk(Response->GetResponseCode())) {
					FString Url = Response->GetContentAsString().Replace(TEXT("\""), TEXT(""));
					UE_LOG(LogTemp, Log, TEXT("Ready to upload via %s"), *Url);
					UploadRequestInfo->UploadURL = Url;
					this->SendFile(UploadRequestInfo);
				} else {
					UE_LOG(LogTemp, Error, TEXT("Unexpected response [%d]: %s"), Response->GetResponseCode(), *Response->GetContentAsString());
					Failed = true;
				}

			} else if (UploadRequestInfo->State.Equals("mkdir")) {
				if(EHttpResponseCodes::IsOk(Response->GetResponseCode()))
				{
					UE_LOG(LogTemp, Log, TEXT("Remote folder created"));
					this->GetUploadLink(UploadRequestInfo);
				}else{
					UE_LOG(LogTemp, Error, TEXT("Unexpected response [%d]: %s"), Response->GetResponseCode(), *Response->GetContentAsString());
					Failed = true;
				}

			} else if (UploadRequestInfo->State.Equals("sending")) {
				if(EHttpResponseCodes::IsOk(Response->GetResponseCode())){
					FString UploadID = Response->GetContentAsString();

					if(UploadID == "0000000000000000000000000000000000000000")
					{
						UE_LOG(LogTemp, Error, TEXT("Upload failed. File ID: %s"), *UploadID);
						UploadRequestInfo->Callback.ExecuteIfBound(false, UploadRequestInfo->LocalPath);
					}else{
						UE_LOG(LogTemp, Log, TEXT("Upload finished. File ID: %s"), *UploadID);
						UploadRequestInfo->Callback.ExecuteIfBound(true, UploadRequestInfo->LocalPath);
					}

				}else{
					UE_LOG(LogTemp, Error, TEXT("Unexpected response [%d]: %s"), Response->GetResponseCode(), *Response->GetContentAsString());
					Failed = true;
				}

			} else {
				UE_LOG(LogTemp, Warning, TEXT("Unknown state"));
			}
		} else {
			UE_LOG(LogTemp, Error, TEXT("Could not find request info :("));
			Failed = true;
		}
	}else{
		UE_LOG(LogTemp, Error, TEXT("HTTP request failed!"));
		Failed = true;
	}

	if(Failed){
		UploadRequestInfo->Callback.ExecuteIfBound(false, UploadRequestInfo->LocalPath);
	}

	if (UploadRequestInfo != NULL) {
		this->UploadLinkRequests.Remove(Request);
	}

}

void USeafileClient::SendFile(FUploadRequestInfo* UploadInfo) {
	UploadInfo->State = "sending";

	TSharedRef<IHttpRequest> Request = this->CreateRequest(UploadInfo->UploadURL, "POST");
	this->UploadLinkRequests.Add(Request, UploadInfo);

	FString PartSeparator = "------------------------sVdFR2ccL8e72M4e";
	Request->SetHeader(TEXT("Content-Type"), TEXT("multipart/form-data; boundary=" + PartSeparator));

	FString FileContents;
	FFileHelper::LoadFileToString(FileContents, *UploadInfo->LocalPath);

	FString Filename = FPaths::GetCleanFilename(UploadInfo->LocalPath);

	if (UploadInfo->NameOverride != "") {
		Filename = UploadInfo->NameOverride;
	}
	FString Content = PartSeparator;
	PartSeparator = "\r\n" + PartSeparator;
	Content += "\r\nContent-Disposition: form-data; name=\"file\"; filename=\"" + Filename + "\"";
	Content += "\r\nContent-Type: text/plain\r\n\r\n" + FileContents + PartSeparator;
	Content += "\r\nContent-Disposition: form-data; name=\"parent_dir\"\r\n\r\n" + UploadInfo->RemotePath + PartSeparator;
	Request->SetContentAsString(Content);

	Request->OnProcessRequestComplete().BindUObject(this, &USeafileClient::HandleGenericHTTPResponse);
	Request->ProcessRequest();
}

void USeafileClient::GetUploadLink(FUploadRequestInfo* UploadInfo){
	UE_LOG(LogTemp, Log, TEXT("Creating request to get upload link"));
	UploadInfo->State = "gettingLink";

	TSharedRef<IHttpRequest> Request = this->CreateRequest("repos/" + UploadInfo->RepoID + "/upload-link/?p=" + UploadInfo->RemotePath);

	this->UploadLinkRequests.Add(Request, UploadInfo);
	Request->OnProcessRequestComplete().BindUObject(this, &USeafileClient::HandleGenericHTTPResponse);
	Request->ProcessRequest();
}


void USeafileClient::Mkdir(FUploadRequestInfo* UploadInfo){
	UE_LOG(LogTemp, Log, TEXT("Creating request to mkdir"));
	UploadInfo->State = "mkdir";

	TSharedRef<IHttpRequest> Request = this->CreateRequest(
		"repos/" + UploadInfo->RepoID + "/dir/?p=" + UploadInfo->RemotePath,
		"POST"
	);
	this->UploadLinkRequests.Add(Request, UploadInfo);

	Request->SetContentAsString("operation=mkdir");
	Request->OnProcessRequestComplete().BindUObject(this, &USeafileClient::HandleGenericHTTPResponse);
	Request->ProcessRequest();
}

void USeafileClient::UploadFile(FString LocalPath, FString RemotePath, FString RepoID, FString NameOverride, const FUploadComplete& OnUploadComplete){
	if(RepoID.Equals(""))
	{
		RepoID = this->DefaultRepoID;
	}

	if (!RemotePath.StartsWith("/"))
	{
		RemotePath = "/" + RemotePath;
	}

	if (!RemotePath.EndsWith("/"))
	{
		RemotePath += "/";
	}

	FUploadRequestInfo* UploadInfo = new FUploadRequestInfo(LocalPath, RemotePath, RepoID, NameOverride);
	UploadInfo->Callback = OnUploadComplete;

	UE_LOG(LogTemp, Log, TEXT("Starting upload process..."));

	this->GetUploadLink(UploadInfo);
}
