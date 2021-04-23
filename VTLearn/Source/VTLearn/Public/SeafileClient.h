#pragma once

#include "Runtime/Online/HTTP/Public/Http.h"

#include "Runtime/Core/Public/Delegates/Delegate.h"

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "Kismet/BlueprintAsyncActionBase.h"
#include "SeafileClient.generated.h"

DECLARE_DYNAMIC_DELEGATE_OneParam(FHTTPActionComplete, bool, Success);
DECLARE_DYNAMIC_DELEGATE_TwoParams(FUploadComplete, bool, Success, FString, Filename);

USTRUCT(BlueprintType)
struct FUploadRequestInfo {
	GENERATED_USTRUCT_BODY()

public:
	FString State = "";
	FString UploadURL = "";

	UPROPERTY()
	FString LocalPath;

	UPROPERTY()
	FString RemotePath;

	UPROPERTY()
	FString RepoID;

	UPROPERTY()
	FString NameOverride;

	UPROPERTY()
	FUploadComplete Callback;

	FUploadRequestInfo(FString InLocalPath="", FString InRemotePath="", FString InRepoID="", FString InNameOverride="")
		: LocalPath(InLocalPath), RemotePath(InRemotePath), RepoID(InRepoID), NameOverride(InNameOverride) {}
};

UCLASS(BlueprintType, Config=VTSettings)
class VTLEARN_API USeafileClient : public UObject {
	GENERATED_BODY()

public:

private:
	TSharedRef<IHttpRequest> CreateRequest(FString Route, FString Method="GET");
	TMap<FHttpRequestPtr, FHTTPActionComplete> AuthenticationCallbacks;
	TMap<FHttpRequestPtr, FUploadRequestInfo*> UploadLinkRequests;

public:
	void GetUploadLink(FUploadRequestInfo* UploadInfo);
	void Mkdir(FUploadRequestInfo* UploadInfo);
	void SendFile(FUploadRequestInfo* UploadInfo);

	UPROPERTY()
	FString Token;

	UPROPERTY(Config)
	FString DefaultRepoID;

	UPROPERTY()
	FString BaseURL;

	UFUNCTION(BlueprintCallable, meta = (DisplayName = "Authenticate"), Category = "DataTools | Seafile")
	void Authenticate(FString Server, FString Username, FString Password, const FHTTPActionComplete& OnAuthComplete);

	UFUNCTION(BlueprintCallable, meta = (DisplayName = "Upload file"), Category = "DataTools | Seafile")
	void UploadFile(FString LocalPath, FString RemotePath, FString RepoID, FString NameOverride, const FUploadComplete& OnUploadComplete);

	void HandleAuthResponse(FHttpRequestPtr Request, FHttpResponsePtr Response, bool Success);
	void HandleGenericHTTPResponse(FHttpRequestPtr Request, FHttpResponsePtr Response, bool Success);
};